/*

Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009
   Chris J Arges
   Everton Constantino
   Tom Gall
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp
 
 *
 * Various test utilities.
 *
 */

#define test_print(...) { printf("[%s]: ",__FUNCTION__);printf(__VA_ARGS__);}

/* memprint mimics xxd */
static inline void test_memprint(char *addr, long len)
{
	long i;

	for (i = 0; i < len; i++) {
		if ((i % 16) == 0) {
			if (i)
				printf("\n");
			printf("%08lx: ", (long)(addr + i));
		}
		printf("%02x", (unsigned char)*(addr + i));
		if (((i + 1) % 2) == 0)
			printf(" ");
	}
	printf("\n");
}
