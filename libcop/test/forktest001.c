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
#include <unistd.h>
#include <libcop.h>

int main(int argc, char *argv[])
{
	pid_t pid = fork();
	printf("* fork returns %d\n",pid);
	if (pid == 0) {
		printf("* child(%d) rand %x\n", getpid(), cop_rand());
	} else {
		printf("* parent(%d) rand %x\n", getpid(), cop_rand());
	}
	return 0;
}
