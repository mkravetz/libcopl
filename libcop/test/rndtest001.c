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
*/

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <libcop.h>

int main(int argc, char **argv)
{
	int fail = 0, i;
	long rndValue;

	/* Here we use a synchronous call */
	errno = 0;
	for (i = 0; i < 10; i++) {
		rndValue = cop_random();
		if (errno != 0 || rndValue == 0)
			fail++;

		printf("Seed returned is %x, errno=%x\n", rndValue, errno);
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, 1);
	return fail;
}
