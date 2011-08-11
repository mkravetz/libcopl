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
#include <libcop.h>

int main(int argc, char *argv[])
{
	int i;
	long value, value2;

	cop_trans_t cop_transaction_tag;
	cop_trans_t cop_transaction_tag2;

	cop_transaction_tag = cop_start(RANDOM_NUMBER_COPRO);
	cop_transaction_tag2 = cop_start(RANDOM_NUMBER_COPRO);

	for (i = 0; i < 100; i++) {
		value = cop_random(cop_transaction_tag);
		value2 = cop_random(cop_transaction_tag2);
		printf("values returned are %d %d\n", value, value2);
	}

	cop_end(cop_transaction_tag);

	cop_end(cop_transaction_tag2);

	return 0;
}
