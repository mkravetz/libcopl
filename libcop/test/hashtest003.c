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
#include <string.h>
#include <pthread.h>
#include <libcopmalloc.h>
#include "libcop.h"

//#define NUM_TESTS 300
/* FIXME DD1 */
#define NUM_TESTS 30

typedef struct {
	int id;
} parameters;

int fail = 0;
pthread_mutex_t mutex;
const char edda[] = " \
1. Hearing I ask | from the holy races, \
From Heimdall's sons, | both high and low; \
Thou wilt, Valfather, | that well I relate \
Old tales I remember | of men long ago. \
\ 
2. I remember yet | the giants of yore, \
Who gave me bread | in the days gone by; \
Nine worlds I knew, | the nine in the tree \
With mighty roots | beneath the mold. \ 
\
3. Of old was the age | when Ymir lived;\
Sea nor cool waves | nor sand there were;\
Earth had not been, | nor heaven above,\
But a yawning gap, | and grass nowhere.\
\
4. Then Bur's sons lifted | the level land,\
Mithgarth the mighty | there they made;\
The sun from the south | warmed the stones of earth,\
And green was the ground | with growing leeks.\
\
5. The sun, the sister | of the moon, from the south\
Her right hand cast | over heaven's rim;\
No knowledge she had | where her home should be,\
The moon knew not | what might was his,\
The stars knew not | where their stations were.\
\
6. Then sought the gods | their assembly-seats,\
The holy ones, | and council held;\
Names then gave they | to noon and twilight,\
Morning they named, | and the waning moon,\
Night and evening, | the years to number.\
\
7. At Ithavoll met | the mighty gods,\
Shrines and temples | they timbered high;\
Forges they set, and | they smithied ore,\
Tongs they wrought, | and tools they fashioned.\
\
8. In their dwellings at peace | they played at tables,\
Of gold no lack | did the gods then know,--\
Till thither came | up giant-maids three,\
Huge of might, | out of Jotunheim.\
\
9. Then sought the gods | their assembly-seats,\
The holy ones, | and council held,\
To find who should raise | the race of dwarfs\
Out of Brimir's blood | and the legs of Blain.\
\
10. There was Motsognir | the mightiest made\
Of all the dwarfs, | and Durin next;\
Many a likeness | of men they made,\
The dwarfs in the earth, | as Durin said.\
\
11. Nyi and Nithi, | Northri and Suthri,\
Austri and Vestri, | Althjof, Dvalin,\
Nar and Nain, | Niping, Dain,\
Bifur, Bofur, | Bombur, Nori,\
An and Onar, | Ai, Mjothvitnir.\
\
12. Vigg and Gandalf) | Vindalf, Thrain,\
Thekk and Thorin, | Thror, Vit and Lit,\
Nyr and Nyrath,-- | now have I told--\
Regin and Rathsvith-- | the list aright.\
\
13. Fili, Kili, | Fundin, Nali,\
Heptifili, | Hannar, Sviur,\
Frar, Hornbori, | Fræg and Loni,\
Aurvang, Jari, | Eikinskjaldi.\
\
14. The race of the dwarfs | in Dvalin's throng\
Down to Lofar | the list must I tell;\
The rocks they left, | and through wet lands\
They sought a home | in the fields of sand.\
\
15. There were Draupnir | and Dolgthrasir,\
Hor, Haugspori, | Hlevang, Gloin,";

void hash_test(void *args)
{
	/* get the parameters */
	parameters *p = (parameters *) args;
	int id = p->id;
    mapped_memory_pool_t cc_pool = cop_cc_pool();
    char *digested_edda = (char *) cop_malloc (cc_pool, sizeof(edda), 1);
    char *cop_edda = (char *) cop_malloc (cc_pool, sizeof(edda), 1);
	memcpy(cop_edda, edda, sizeof(edda));
    errorn e = -1;

    unsigned int sel = ((unsigned int)cop_rand()) % 4;
    switch( sel ){
        case 0:
            e = cop_md5( cop_edda, sizeof(edda), digested_edda, NULL, COP_FLAG_ONESHOT);
            break;
        case 1:
            e = cop_sha1( cop_edda, sizeof(edda), digested_edda, NULL, COP_FLAG_ONESHOT);
            break;
        case 2:
            e = cop_sha256( cop_edda, sizeof(edda), digested_edda, NULL, COP_FLAG_ONESHOT);
            break;
        case 3:
            e = cop_sha512( cop_edda, sizeof(edda), digested_edda, NULL, COP_FLAG_ONESHOT);
            break;
    }
    
    cop_free(cc_pool, digested_edda);
    cop_free(cc_pool, cop_edda);
    if(e){
    	pthread_mutex_lock(&mutex);
	fail++;
	pthread_mutex_unlock(&mutex);
        printf("(%X) Failed with error %d\n", id, e );
    } else {
	printf("(%X) Finished successfully\n", id);
    }
}

int main(int argc, char *argv[])
{
	int i;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	pthread_t *threads = NULL;
	parameters params;

	//---SPAWNING THREADS---
	threads = (pthread_t *) malloc(NUM_TESTS * sizeof(pthread_t *));
	for (i = 0; i < NUM_TESTS; i++) {
		params.id = i;
		if (pthread_create (&threads[i], NULL, (void *)&hash_test, &params))
			fail++;
	}

	for (i = 0; i < NUM_TESTS; i++) {
		if (pthread_join(threads[i], NULL))
			fail++;
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS);
	return 0;
}
