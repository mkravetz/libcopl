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

#define MAX_I 3
#define MAX_J 3

float M[MAX_I][MAX_J] = { {1.0, 2.0, 3.0},
{4.0, 5.0, 6.0},
{7.0, 8.0, 9.0}
};

int act_i = 0;
float *final_s = NULL;

float process_main()
{
	int j = 0;
	float sum = 0;

	for (j = 0; j < MAX_I; j++) {
		sum += M[act_i][j];
	}

	return sum;
}

int main(int argc, char *argv[])
{
	int i = 0;
	int fildes[2], fildes2[2];
	int status = pipe(fildes);

	final_s = (float *)malloc(sizeof(float));

	if (status == -1 || final_s == NULL) {
		return -1;
	}

	status = pipe(fildes2);
	if (status == -1) {
		return -1;
	}

	for (i = 0; i < 5; i++) {
		pid_t pid = fork();
		if (pid == 0) {
			printf("#%X\n", cop_rand());
			close(fildes[1]);
			close(fildes2[0]);

			if (act_i < MAX_I) {
				float s = process_main();

				printf("%f\n", s);

				read(fildes[0], final_s, sizeof(float));
				*final_s = *final_s + s;
				write(fildes2[1], final_s, sizeof(float));
			} else {
				printf("DONE[%d]\n", act_i);
			}

			close(fildes[0]);
			close(fildes2[1]);

			exit(EXIT_SUCCESS);
		} else {
			sleep(0.5);
			if (act_i < MAX_I) {
				write(fildes[1], final_s, sizeof(float));
				read(fildes2[0], final_s, sizeof(float));
			}
			act_i++;
		}
	}

	printf("Final sum = %f\n", *final_s);

	free(final_s);

	return 0;
}
