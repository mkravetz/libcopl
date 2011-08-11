/* 
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Chris J Arges
   Everton Constantino
   Tom Gall
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp
*/

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "asm/copro-driver.h"
#include "asm/copro-regx.h"
#include <libcop.h>
#include <libcopl/cop_sys.h>
#include <libcopl/rnd.h>
#include <libcopl/icswx.h>

struct rnd_cxb {
	struct cop_crb crb;
	struct cop_csb csb;
	uint8_t _pad[48];
	uint64_t result;
};

#define R_BUF_SIZE 512

int main(int argc, char **argv)
{
	int fail = 0, i,j, num_events, n_tests = 10;
	long rndValue;
	int retv, rc;
	char r_buf[R_BUF_SIZE];
	uint64_t instance_list[10];
	int max_events=10;
	struct epoll_event ev, events[10];
	struct cop_ctx ctx;
	struct rnd_cxb *cxb;
	int epollfd;

	/*
	 * open/setup for coprocessor calls
	 */
	if (cop_open(&ctx, COP_RNG)) {
		fprintf(stderr,"cop_open failure\n");
		exit(1);
	}
	if (cop_get_instances(&ctx, instance_list, sizeof(instance_list))<1) {
		fprintf(stderr,"cop_get_instances failure\n");
		exit(1);
	}
	if (cop_bind_instance(&ctx, instance_list[0])) {
		fprintf(stderr,"cop_bind_instance failure\n");
		exit(1);
	}
	if (cop_alloc_imq(&ctx, 0x10000)) {
		fprintf(stderr,"COPRO_IOCTL_ALLOC_IMQ failure\n");
		exit(1);
	}
	if (cop_enable_imq(&ctx)) {
		fprintf(stderr,"COPRO_IOCTL_ENABLE_IMQ failure\n");
		exit(1);
	}

	/*
	 * setup for epoll
	 */
	epollfd = epoll_create(10);
	if (epollfd == -1) {
		fprintf(stderr,"error on epoll create\n");
		exit(1);
	}
	ev.events = EPOLLIN;
	ev.data.fd = ctx.fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ctx.fd, &ev)==-1) {
		fprintf(stderr,"error on adding fd\n");
		perror("");
		exit(1);
	}

	/*
	 * Allocate coproc control blocks
	 */
	if (posix_memalign(&cxb, 128, sizeof(struct rnd_cxb))) {
		fprintf(stderr,"posix_memalign error\n");
		exit(1);
	}
	if (cop_pbic_map(&ctx, cxb, sizeof(struct rnd_cxb), COP_MAP_ALL_FLAGS)){
		fprintf(stderr,"posix_memalign error\n");
		exit(1);
	}


	/*
	 * loop for multiple calls
	 */
	for (i = 0; i < n_tests; i++) {
		memset(cxb, 0, sizeof(struct rnd_cxb));
		cop_set_csbaddr(&cxb->crb, &cxb->csb);
		if (cop_random_setup(&cxb->crb, &ctx, &cxb->result,
							sizeof(uint64_t))) {
			fprintf(stderr,"cop_random_setup error\n");
			exit(1);
		}
		cop_crb_set_comp_mthd(&cxb->crb, COP_COMP_MTHD_INTERRUPT);

		if (icswx(&cxb->crb)) {
			fprintf(stderr,"icswx failure\n");
			exit(1);
		}

		num_events=epoll_wait(epollfd, events, max_events, 1000);

		if (num_events == -1) {
			fprintf(stderr,"epoll wait failed out\n");
			fail++;
		} else if (num_events == 0) {
			fprintf(stderr,"epoll wait no event?!?!\n");
			fail++;
		}

		/*
		 * rndValue = cop_get_rnd(session);
		 */
		rndValue = *((long *)cop_get_tdptr(&cxb->crb));
		printf("Seed returned is %x\n", rndValue);
		if (!rndValue)
			fail++;

		/*
		 * read (imq) data from coproc fd
		 */
		do {
			rc = read(ctx.fd, r_buf, R_BUF_SIZE);
			if (rc < 0) {
				fprintf(stderr,"coproc read failure\n");
				exit(1);
			}
		} while(rc == R_BUF_SIZE);

	}

	close(epollfd);
	printf("=== %s: %d/%d failures ===\n", argv[0], fail, n_tests);
	return fail;
}
