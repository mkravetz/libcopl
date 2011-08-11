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

#include <stdio.h>
#include <stdlib.h>

#include <libcop.h>

int main(int argc, char *argv[])
{
	int result = 0;

	result = coprocessor_exists(REG_EX_COPRO);

	if (result)
		printf("Test successful on wsp hardware\n");

	else
		printf("Test successful off wsp hardware \n");

	return result;
}
