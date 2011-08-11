/*Common framework for use by all performance tests*/
/*Anil Krishna and Timothy Heil*/

/* Modifications for libcopl by Chris J Arges */

#include "bma2libcopl.h"
#include "threads_posix.h"

#ifdef DEBUG_ON
#define debug_printf(...) printf(__VA_ARGS__);
#else
#define debug_printf(...) ;;
#endif

#define CHECK(statement, error_message, error_ret) {\
    if (!(statement)) {\
        fprintf(stderr, "[%s:%d]: %s\n", __func__, __LINE__, error_message);\
	return error_ret;\
    }\
}

#define ICSWX_ALIGN	128

/* Inter-thread communication structure.*/
/* Each worker gets its own instance.*/
struct thParms {
	int nTh;		/* How many worker threads. */
	int id;			/* My ID. */
	struct cop_ctx *ctx;	/* pointer to copro structure */
	int reqs;	/* number of requests made by each thread */
	int pktb;	/* input request size per request, in bytes */

	/* Timing. */
	uint64_t wakeup;
	uint64_t start_loop[2];
	uint64_t *start_icswx;
	uint64_t *send_icswx;
	uint64_t *icswx_sent;
	int *num_try_icswx;
	uint64_t *icswx_complete;
	int *icswx_complete_trys;
};

/* pthread globals */
pthread_mutex_t mutex;
pthread_barrier_t barr;

static int volatile ping = -1;

static int test(struct thParms *th)
{
	/* test variables */
	const int id = th->id;
	(void)id;
	int loop;
	int Rq;
	th->wakeup = mftb();
	int reqs = th->reqs;
	int pktb = th->pktb;
	struct cop_ctx *ctx = th->ctx;

	/* input/output buffers */
	unsigned char *input;
	unsigned input_sz = pktb;
	unsigned char *output;
	unsigned output_sz = ALIGN_UP(input_sz, pktb);

	/* allocate/zero chunk for crbs */
	perf_cxb *cxb;
	cxb = dmalloc(ctx, ICSWX_ALIGN, sizeof(perf_cxb) * reqs);
	CHECK(cxb, "dalloc cxbs failed", -1);
	memset(cxb, 0, sizeof(perf_cxb) * reqs);

	/* allocate data */
	input = dmalloc(ctx, ICSWX_ALIGN, input_sz);
	CHECK(input, "dalloc input failed", -1);
	output = dmalloc(ctx, ICSWX_ALIGN, output_sz);
	CHECK(output, "dalloc output failed", -1);

	debug_printf("Input Size : %d\n Output Size : %d\n", input_sz, output_sz);

	/* Initialize structures for all requests. */
	for (Rq = 0; Rq < reqs; Rq++) {

		/* Setup CSB/CRB */
		cop_set_csbaddr(&cxb[Rq].crb, &cxb[Rq].csb);
		SETUP_FUNC();

		/*debug_printf("T%02d : CRB %2d Source @ 0x%08llx x %d\n",
		       id, Rq, (unsigned long long)&cxb[Rq].crb.src_dde.data, input_sz);
		debug_printf("T%02d : CRB %2d Target @ 0x%08llx x %d\n",
		       id, Rq, (unsigned long long)&cxb[Rq].crb.tgt_dde.data, output_sz);
		*/
	}
	debug_printf("T%02d: Init complete\n", id);

	/* Send all requests. Loop first to warm up cache, then profile second run. */
	for (loop = 0; loop < 2; loop++) {
		/* Wait here until all threads arrive. */
		pthread_barrier_wait(&barr);

		int startCRB = loop * (reqs / 2);
		int endCRB = startCRB + (reqs / 2);
	//	if (id == 0 && loop == 1)
	//		startPMU(1);
	
		for (Rq = startCRB; Rq < endCRB; Rq++) {
			int rc;
			int trials = 0;

			/* Send request and retry if not accepted. */
			debug_printf("T%02d: CRB %d\n", id, Rq);

			th->start_icswx[Rq] = mftb();
			do {
				th->send_icswx[Rq] = mftb();
				trials++;
				rc = icswx(&(cxb[Rq].crb));

			} while (rc != 0);

			th->icswx_sent[Rq] = mftb();
			th->num_try_icswx[Rq] = trials;
		}
	//	if (id == 0 && loop == 1)
	//		stopPMU(input_sz);

		debug_printf("T%02d: CRBs sent\n", id);

		for (Rq = startCRB; Rq < endCRB; Rq++) {
			int trials = 1;
			while (!cxb[Rq].csb.cw.valid) {
				trials++;
				clobber();
			}
			th->icswx_complete[Rq] = mftb();
			th->icswx_complete_trys[Rq] = trials;
		}
		debug_printf("T%02d: CRBs complete\n", id);
	}
	pthread_barrier_wait(&barr);

	/* free everything */
	dfree(ctx, input, input_sz);
	dfree(ctx, output, output_sz);
	dfree(ctx, cxb, sizeof (perf_cxb) * reqs);

	debug_printf("T%02d: Returning\n", id);
	return 0;
}

static void work(void *arg)
{
	struct thParms *th = (struct thParms *)arg;
	ping = th->id;
	int fail = test(th);
	(void)fail;
	debug_printf("T%02d: fail=%d\n", th->id, fail);
	thread_exit();
}

static int mt_testos(int argc, char *argv[])
{
	int sys_nTh = thread_count();
	int self = thread_self();
	assert(self == 0);
	int i;
	int Rq;
	//uint64_t main_start = mftb();
	
	/* default test parameters */
	int nTh = 8;
	int REQS = 144;
	int PKTB = 256;

	/* read in inputs if possible, otherwise do defaults */
	if (argc == 4) {
		int t_nTh = atoi(argv[1]);
		if (t_nTh > 0 && t_nTh <= MAX_THREADS)
			nTh = t_nTh;
		else
			printf("# threads must be between 0 and %d\n", MAX_THREADS);

		int t_REQS = atoi(argv[2]);
		if (t_REQS > 0 && t_nTh <= 4096)
			REQS = t_REQS;
		else
			printf("# of requests must be between 0 and 4096\n");

		int t_PKTB = atoi(argv[3]);
		if (t_PKTB > 0 && t_PKTB <= 16384)
			PKTB = t_PKTB;
		else
			printf("packet size must be between 0 and 16384\n");

	}

	/* init the cop ctx */
	struct cop_ctx ctx;
	cop_init(&ctx, COP_SYM_CRYPTO);

	/* make sure we get all the threads */
	printf("\nRunning test %s\n", __BASE_FILE__);
	printf("System has %d threads.  Running %d requester threads,\n"
		"on %d byte packets with %d requests per thread.\n",
		sys_nTh, nTh, PKTB, REQS);
	if (nTh == 1) {
		puts(" Sorry, only one thread configured\n");
		return -1;
	}

	/* setup pthread sync primitives - cja */
	pthread_mutex_init(&mutex, NULL);
	pthread_barrier_init(&barr, NULL, nTh);

	/* allocate and setup thread structures, then start */
	int context = 0;
	struct thParms *thP = malloc(nTh * sizeof (struct thParms));
	for (i = 0; i < nTh; i++) {
		thP[i].id = i;
		thP[i].nTh = nTh - 1;	/* Less one starter thread. */
		thP[i].ctx = &ctx;

		/* setup test parameters */
		thP[i].reqs = REQS;
		thP[i].pktb = PKTB;

		/* Init all timestamps to ensure they are all properly set. */
		thP[i].wakeup = -1;
		thP[i].start_loop[0] = -1;
		thP[i].start_loop[1] = -1;

		/* allocate space for counters */
		thP[i].start_icswx = malloc(REQS * sizeof(uint64_t));
		thP[i].send_icswx = malloc(REQS * sizeof(uint64_t));
		thP[i].icswx_sent = malloc(REQS * sizeof(uint64_t));
		thP[i].num_try_icswx = malloc(REQS * sizeof(int));
		thP[i].icswx_complete = malloc(REQS * sizeof(uint64_t));
		thP[i].icswx_complete_trys = malloc(REQS * sizeof(int));

		for (Rq = 0; Rq < REQS; Rq++) {
			thP[i].start_icswx[Rq] = -1;
			thP[i].send_icswx[Rq] = -1;
			thP[i].icswx_sent[Rq] = -1;
			thP[i].num_try_icswx[Rq] = -1;
			thP[i].icswx_complete[Rq] = -1;
			thP[i].icswx_complete_trys[Rq] = -1;
		}
		//context = i * (sys_nTh / nTh) + 4;
		context = i % sys_nTh;
		tmesg("Start thread: %d of %d on context %d\n", i, nTh,
		      context);
		/* Run on Core 1 of each Node. */
		thread_start(context, work, (void *)&thP[i]);

		//TODO: look at pthread_cond_wait, instead of ping
		/* Wait for thread to get going. */
		while (ping != i)
			clobber();
	}

	/* join all the threads */
	for (i = 0; i < nTh; i++) {
		int rc;
		//context = i * (sys_nTh / nTh) + 4;
		context = i % sys_nTh;
		tmesg("Join thread: %d on context %d\n", i, context);
		rc = thread_join(i);
		tmesg("Joined thread: %d\n", i);
		passfail(rc != context, NULL);
	}

	/* Output time stamps. */
	/*printf("Ttt, CRB,%s,%-8s,Val\n", "Tag", "Time");
	 * printf("T%02d,%4d,%s,%8lld,%2d\n",
	 * 99, 99, "MAIN_START", main_start, 0);
	 * 
	 * for (i = 0; i < nTh; i++) {
	 * int loop;
	 * printf("T%02d,%4d,%s,%8lld,%2d\n", i, 99, 
	 * "WAKEUP      ", thP[i].wakeup, 0);
	 * for (loop = 0; loop < 2; loop++) {
	 * printf("T%02d,%4d,%s,%8lld,%2d\n", i, 99, 
	 * "START_LOOP  ", thP[i].start_loop[loop], 0);
	 * }
	 * 
	 * for (Rq = 0; Rq < REQS; Rq++) {
	 * printf("T%02d,%4d,%s,%8lld,%2d\n", i, Rq, 
	 * "START_ICSWX ", thP[i].start_icswx[Rq], 0);
	 * printf("T%02d,%4d,%s,%8lld,%2d\n", i, Rq,
	 * "SEND_ICSWX  ", thP[i].send_icswx[Rq],
	 * thP[i].num_try_icswx[Rq]);
	 * printf("T%02d,%4d,%s,%8lld,%2d\n",
	 * i, Rq, "ICSWX_SENT  ", thP[i].icswx_sent[Rq], 0);
	 * }
	 * for (Rq = 0; Rq < REQS; Rq++) {
	 * printf("T%02d,%4d,%s,%8lld,%2d\n",
	 * i, Rq, "ICSWX_COMP  ", thP[i].icswx_complete[Rq],
	 * thP[i].icswx_complete_trys[Rq]);
	 * }
	 * } */

	/* sort and output results */
	uint64_t min;
	int Th, iter, minTh = 0, minRq = 0;
	uint64_t *sorted = (uint64_t *)malloc(nTh * REQS * sizeof (uint64_t));
	for (iter = 0; iter < nTh * REQS; iter++) {
		min = -1;
		for (Th = 0; Th < nTh; Th++) {
			for (Rq = 0; Rq < REQS; Rq++) {
				if (thP[Th].send_icswx[Rq] < min) {
					min = thP[Th].send_icswx[Rq];
					minTh = Th;
					minRq = Rq;
				}
			}
		}
		sorted[iter] = min;
		thP[minTh].send_icswx[minRq] = -1;
	}

	printf("Timebase Throughput = %llu bps\n",
	       (unsigned long long)100 * PKTB * 8 * 33333333 / (sorted[200] -
								sorted[100]));
	printf("Timebase Throughput upped = %llu bps\n",
	       (unsigned long long)100 * PKTB * 8 * 42592592 / (sorted[200] -
								sorted[100]));

	/* free everything */
	for (i = 0; i < nTh; i++) {
		free(thP[i].start_icswx);
		free(thP[i].send_icswx);
		free(thP[i].icswx_sent);
		free(thP[i].num_try_icswx);
		free(thP[i].icswx_complete);
		free(thP[i].icswx_complete_trys);
	}
	free(thP);
	free(sorted);
	return 0;
}
