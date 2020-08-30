#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>

/* 
 * gcc -ansi -pedantic -Werror -Wall -O3 -lpthread valid3.c -o b.out
 * ./b.out
 */

/*
0x178b JT reg0 0x1793
0x178e ADD reg0 reg1 1
0x1792 RET
0x1793 JT reg1 0x17a0
0x1796 ADD reg0 reg0 32767
0x179a SET reg1 reg7
0x179d CALL 0x178b
0x179f RET
0x17a0 PUSH reg0
0x17a2 ADD reg1 reg1 32767
0x17a6 CALL 0x178b
0x17a8 SET reg1 reg0
0x17ab POP reg0
0x17ad ADD reg0 reg0 32767
0x17b1 CALL 0x178b
0x17b3 RET
...
0x156b SET reg0 4
0x156e SET reg1 1
0x1571 CALL 0x178b
...
0x1573 EQ reg1 reg0 6 ; reg1 needs to equal '5' when call returns
*/

static int found = 0; /* terminate running */
static int r7_next = 0;
pthread_mutex_t lock;

int next() {
	int nc;
	pthread_mutex_lock(&lock);
	nc = r7_next;
	r7_next++;
	if(r7_next >= 32768) {
		r7_next = 0;
	}
	pthread_mutex_unlock(&lock);
	return nc;
}

int valid(int r0, int r1, int r7) {
	if (r0 == 1 && r1 == 0)
		return r7;
	if (r0 == 1 && r1 > r0)
		return (r1 + r7) % 32768;
	while (r0 != 0){
		if (r1 != 0)
			return valid(r0-1, valid(r0,r1-1, r7)+1, r7);
		return valid(r0-1, r7, r7);
	}
	return r1 % 32768;
}

void * valid_c(void * t) {
	while (!found) {
		int res, k;
		k = next();
		res = valid(4, 1, k);
		printf("%d, %d\n", k, res);
		if(res == 5) {  /* reg1 needs to equal '5' when call returns */
			printf("Solution K is: %d", k);
			found = 1;
		}
	}
	return NULL;
}

int main(int argc, char**argv) {

	int num_threads = 12;
	int i = 0;

	pthread_t *threads;
	threads = (pthread_t *) malloc (num_threads * sizeof (pthread_t));

	while(!found) {
		for(i=0; i<num_threads; i++) {
			if (0 != pthread_create (&threads[i], NULL, valid_c, (void *) NULL))
				exit (EXIT_FAILURE);
		}
		for (i=0; i<num_threads; i++) {
			if (0 != pthread_join (threads[i], NULL))
				exit (EXIT_FAILURE);
		}
	}
	free(threads);
	return 0;
}
