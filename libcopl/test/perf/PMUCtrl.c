#if 0
/************* PBIC ********************/
/* Performance Counter 0-7 Mux Control Register - 9.2.1.1 and Table 9-3*/
#define PBIC_MMIO_BASE_ADDR		iomap(0x4A51488000ULL)
#define PBIC_MUX_CTRL_0_ADDR		iomap(0x4A51488070ULL)
//#define PBIC_MUX_CTRL_0_VAL           0x3B3D3F43FFFFFFFFULL
#define PBIC_MUX_CTRL_0_VAL		0xBDD145484B4E51FFULL
/* Performance Counter Control - 9.2.1.3 */
#define PBIC_COUNTER_CTRL_ADDR		iomap(0x4A51488080ULL)
#define PBIC_COUNTER_CTRL_RESET_VAL	0xFFFFFFFF00000000ULL
#define PBIC_COUNTER_CTRL_VAL		0xFFFF000000000000ULL
#define PBIC_PMU_COUNTER_0_ADDR		iomap(0x4A5148B000ULL)

/* Performance Monitor Control0 Register - 10.1.8.1 */
#define CDM_CTRL_0_ADDR 		CONCAT(A280)
#define CDM_CTRL_0_VAL  		0x0
/* Performance Monitor Control1 Register - 10.1.8.2 */
#define CDM_CTRL_1_ADDR 		CONCAT(A288)
#define CDM_CTRL_1_VAL  		0x0

//---------------------------------------------------------------
static void startPMU(unsigned int doReset)
{
	*(unsigned long long *)PBIC_MUX_CTRL_0_ADDR =
		(unsigned long long)PBIC_MUX_CTRL_0_VAL;
	*(unsigned long long *)PBIC_COUNTER_CTRL_ADDR =
		doReset ? (unsigned long long)PBIC_COUNTER_CTRL_RESET_VAL
		: (unsigned long long)PBIC_COUNTER_CTRL_VAL;
}

//---------------------------------------------------------------

//---------------------------------------------------------------
static void stopPMU(unsigned int s)
{
	*(unsigned long long *)PBIC_COUNTER_CTRL_ADDR = 0;
	volatile unsigned long long *p =
		(unsigned long long *)PBIC_PMU_COUNTER_0_ADDR;
	//volatile unsigned long long *q = (unsigned long long*)PBIC_MMIO_BASE_ADDR;
	//printf("PBus MMIO Base Address Register = 0x%llx\n", *q);
	/*printf("Unit B Direct Hits = %llu\n",*p++);
	 * printf("Unit B Indirect Hits = %llu\n",*(p++));
	 * printf("Unit B Translate Misses = %llu\n",*(p++));
	 * printf("Unit B Indirect Hit Duration = %llu\n",*(p++));
	 * printf("PBIC Cycles = %llu\n",*(p++));
	 * printf("PBIC Cycles = %llu\n",*(p++));
	 * printf("PBIC Cycles = %llu\n",*(p++)); */
	printf("Inbound cop_req commands = %llu \n", *(p++));
	printf("PBIC replies with retry to outbound command = %llu \n",
	       *(p++));
	printf("ORDER Queue B is empty for = %llu cycles\n", *(p++));
	printf("ORDER Queue B is full for = %llu cycles\n", *(p++));
	unsigned long long r = *p;
	printf("Request pushed onto Queue B = %llu times\n", *(p++));
	printf("Request pushed onto Queue B when empty = %llu times\n",
	       *(p++));
	printf("Request pushed onto Queue B when full = %llu times\n", *(p++));
	unsigned long long c = *p;
	printf("PBIC Cycles = %llu\n", *(p++));
	printf("Reqs = %llu, Size = %d, PBIC Cycles = %llu\n", r, s, c);
	printf("PMU Throughput = %llu bps\n", r * s * 8 * 1366666666 / c);
	printf("PMU Throughput upped = %llu bps\n",
	       r * s * 8 * 1746296295 / c);
}

//---------------------------------------------------------------
#endif
