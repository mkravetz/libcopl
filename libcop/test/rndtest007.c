/* 
Licensed Materials - Property of IBM
    Restricted Materials of IBM"
    
    Copyright 2008,2009,2010
    Chris J Arges
    Everton Constantino
    Paulo Vital
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
    int fail = 0, n_tests = 10, i;
    int rndValue;
    cop_session_t *session;
    
    /* Here we use an asynchronous call using session*/
    errno = 0;
    for (i = 0; i < n_tests; i++) {
		session = cop_create_random_session(COMPLETION_STORE);
        session = cop_commit_session_async(session);
		rndValue = (int) cop_get_rnd(session);
    	if( errno ) { 
        	printf("RND Error: %d \n", errno);
			fail++; 
			errno=0;
		}
        printf("Seed returned is %d, errno=%d\n", rndValue, errno);
        
        cop_free_session(session);
    }
    
    printf("=== %s: %d/%d failures ===\n", argv[0], fail, n_tests);
    return fail;
}
