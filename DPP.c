#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define SLEEP_MAX 10000001
unsigned int N, S, T;

//mutex array associated with the forks
pthread_mutex_t *forkMutex;

//char array to store the philosophers' status 
//(Thinking/Eating/Waiting/Terminated)
char **state;

//int array to store IDs of the philosophers 
//who are currently holding a fork  
int *heldBy;

//int array to store IDs of the philosophers
//who were the last one to use a fork 
int *prevUsedBy;

//int variable to check if watcher should continue running
int watcherRun = 1;

void *philosopher(void *tid) {	
	int thisID = *(int*)tid;
	int firstFork = thisID;
	int secondFork = thisID+1;
	
	//philosopher N-1 should pick up fork 0 before fork N-1
	if (thisID == N-1) {
		firstFork = 0;
		secondFork = N-1;
	}
	
	//quit while loop if the ID of this philosopher has been set to -1
	while (*(int*)tid != -1) {			
		state[thisID] = "Thinking";	
		usleep(random() % SLEEP_MAX);
		
		state[thisID] = "Waiting";

		//acquire the first fork 
		pthread_mutex_lock(&forkMutex[firstFork]);
		heldBy[firstFork] = thisID;

		//if the second fork is not yet available
		//and the fist fork was last used by this philosopher 
		//release the first fork while waiting for the second fork
		while (pthread_mutex_trylock(&forkMutex[secondFork]))
			if (prevUsedBy[firstFork] == thisID) {
				heldBy[firstFork] = -1;
				prevUsedBy[firstFork] = -1;
				pthread_mutex_unlock(&forkMutex[firstFork]);
			}
		heldBy[secondFork] = thisID;

		//when the second fork is available and successfully acquired
		//check if the first fork is still held by this philosopher
		//if not, re-acquire the first fork		
		if (heldBy[firstFork] != thisID) {
			pthread_mutex_lock(&forkMutex[firstFork]);
			heldBy[firstFork] = thisID;
		}		

		state[thisID] = "Eating";
		usleep(random() % SLEEP_MAX);		
		
		//release both forks
		heldBy[firstFork] = -1;
		prevUsedBy[firstFork] = thisID;
		pthread_mutex_unlock(&forkMutex[secondFork]);
		heldBy[secondFork] = -1;
		prevUsedBy[secondFork]= thisID;
		pthread_mutex_unlock(&forkMutex[firstFork]);	
	}

	state[thisID] = "Terminated";
	pthread_exit(NULL);
}

void *watcher() {
	int i, th, wa, ea, use, avail;
	while (watcherRun) {
		//wait for 0.5 seconds
		usleep(500000);

		//start printing
		printf("Philo   State               Fork    Held by\n");
		th = 0; wa = 0; ea = 0;	use = 0; avail = 0;	
		for (i=0; i<N; i++) {
			if (heldBy[i] != -1) {
				printf("[%2d]:   %-20s[%2d]:   %d\n",i,state[i],i,heldBy[i]);
				use++;
			} else {
				printf("[%2d]:   %-20s[%2d]:   Free\n",i,state[i],i);
				avail++;
			}
			
			if (strcmp(state[i],"Thinking") == 0)
				th++;
			else if (strcmp(state[i],"Waiting") == 0)
				wa++;
			else if (strcmp(state[i],"Eating") == 0)
				ea++;
 			
		}
		printf("th=%2d Wa=%2d Ea=%2d           Use=%2d  Avail=%2d\n\n\n",th,wa,ea,use,avail);	
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	N = atoi(argv[1]);
	S = atoi(argv[2]);
	T = atoi(argv[3]);
	
	int i;
	int philID[N];
	pthread_t threads[N+1];
	forkMutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*N);
	state = (char**)malloc(sizeof(char*)*N);
	heldBy = (int*)malloc(sizeof(int)*N);
	prevUsedBy = (int*)malloc(sizeof(int)*N);	
	
	//initialize all arrays
	for (i=0; i<N; i++) {
		philID[i] = i;
		pthread_mutex_init(&forkMutex[i],NULL);
		state[i] = "Waiting";
		heldBy[i] = -1;
		prevUsedBy[i] = i;
	}	

	//create and run threads
	srandom(S);
	for (i=0; i<N; i++) {
		pthread_create(&threads[i],NULL,philosopher,(void*)&philID[i]);
	}
	pthread_create(&threads[N],NULL,watcher,NULL);
	
	sleep(T);
	
	//cancel and wait for all philosohper threads to terminate
	for (i=0; i<N; i++)
		philID[i] = -1; //mark that this thread should terminate after
				//completing the current thinking-eating cycle
	
	for (i=0; i<N; i++)
		pthread_join(threads[i],NULL);

	//cancel and wait for watcher thread to terminate	
	watcherRun = 0;
	pthread_join(threads[N],NULL);
	
	//destroy variables and free up memory
	for (i=0; i<N; i++)
		pthread_mutex_destroy(&forkMutex[i]);

	free(forkMutex);
	free(state);
	free(heldBy);
	free(prevUsedBy);
	 
	pthread_exit(NULL);
}
