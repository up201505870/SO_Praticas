#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG 0

typedef struct {
    long *cnt;	/* pointer to shared counter */
    int n;		/* no of times to increment */
    int id;		/* application-specific thread-id */
} targ_t;

pthread_mutex_t mptr = PTHREAD_MUTEX_INITIALIZER;

void *tfun(void *arg) {

    targ_t *targ = (targ_t *) arg;
    int i;
  
    printf("Thread %d starting\n", targ->id);

    printf ("&cnt = %p \n", targ->cnt);

    for(i = 0; i < targ->n ; i++) {
		if( DEBUG ) 
		    printf("b4:  %ld ", *(targ->cnt)); 
	
		(*(targ->cnt))++; 
		/* If cnt is always N*MAX_ITER 
		   Then comment the previous line and 
		   uncomment the following 3 lines 
		   aux = *(targ->cnt);
		   aux++;
		   *(targ->cnt) = aux; 
		   */
		if (DEBUG )
		    printf("\t %ld \n", *(targ->cnt)); 
    }
    printf("Thread %d done\n", targ->id);
    return NULL;
}

void *sfun(void *arg) {

    targ_t *targ = (targ_t *) arg;
    int i;
  
    printf("Thread %d starting\n", targ->id);

    printf ("&cnt = %p \n", targ->cnt);

    for(i = 0; i < targ->n ; i++) {
		if( DEBUG ) 
		    printf("b4:  %ld ", *(targ->cnt)); 
        pthread_mutex_lock(&mptr);
		(*(targ->cnt))++; 
        pthread_mutex_unlock(&mptr);
		/* If cnt is always N*MAX_ITER 
		   Then comment the previous line and 
		   uncomment the following 3 lines 
		   aux = *(targ->cnt);
		   aux++;
		   *(targ->cnt) = aux; 
		   */
		if (DEBUG )
		    printf("\t %ld \n", *(targ->cnt)); 
    }
    printf("Thread %d done\n", targ->id);
    return NULL;
}

int main(int argc, char **argv) {

    pthread_t tids[3];

    if(argc != 2) {
        printf("Wrong use of program.\n");
        exit(0);
    }

    int n = atoi(argv[1]);
    pthread_mutex_init(&mptr, NULL);

    long cnt = 0;

    targ_t arg[3];

    arg[0].id = 1;
    arg[0].cnt = &cnt;
    arg[0].n = n;
    arg[1].id = 2;
    arg[1].cnt = &cnt;
    arg[1].n = n;
    arg[2].id = 3;
    arg[2].cnt = &cnt;
    arg[2].n = n;

    pthread_create(&tids[0], NULL, sfun, &arg[0]);
    pthread_create(&tids[1], NULL, sfun, &arg[1]);
    pthread_create(&tids[2], NULL, sfun, &arg[2]);

    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);
    pthread_join(tids[2], NULL);

    printf("Counter: %ld\n", cnt);

    return 0;
}