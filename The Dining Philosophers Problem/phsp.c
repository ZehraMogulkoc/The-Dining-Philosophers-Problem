#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

enum state {THINKING, HUNGRY, EATING};

char* dst;
int num_phsp;
int min_think;;
int max_think;
int min_dine;
int max_dine;
int num;

struct monitor
{	pthread_mutex_t *mutex;
	pthread_cond_t *s;
	enum state *status;
}condition;

void initialization(int num){

    for(int i = 0; i < num; i++){
        condition.status[i] = THINKING;
    }
       for(int i = 0; i < num; i++){
        pthread_mutex_init(&condition.mutex[i], NULL);
        pthread_cond_init(&condition.s[i], NULL);
    }

}


void test(int i){
    if(condition.status[( i + 1 ) % num_phsp] != EATING){
        if(condition.status[(i + num_phsp - 1) % num_phsp] != EATING){
            if(condition.status[i] == HUNGRY){
                condition.status[i] = EATING;
                printf("Now philosopher %d is eatinng. \n", i );
                pthread_cond_signal(&condition.s[i]);
            }
        }
    }
}
int set_time(int max, int min){
     double rand_time = 0;

    if(strcmp(dst, "uniform")){
        while(rand_time > max || rand_time < min){
            double randd= ((double)rand() / RAND_MAX);
               rand_time = max * randd;
        }
    } else if(strcmp(dst, "exponential")) {
        while(rand_time > max || rand_time < min){
            	rand_time = exp((double) (min + max)/ 2);
        }
    } 
    return (int)rand_time;

}


int Pickup(int i){
    condition.status[i] = HUNGRY;//wants to enter crtical secction
  
    clock_t start = clock();
    test(i);//check the status of left and right philosophers
    int lock = -1;
    
    if(condition.status[i] != EATING){ //the case if it cannat eat
        
        if(condition.status[(i + 1) % num_phsp] == EATING){
        
            lock = (i + 1) % num_phsp;
            pthread_cond_wait(&condition.s[i], &condition.mutex[(i + 1) % num_phsp]);
        
        } else{   
        
            lock = i;
            pthread_cond_wait(&condition.s[i], &condition.mutex[i]);
        
        }

    } else{ //otherwise lock its mutexes
        
        pthread_mutex_lock(&condition.mutex[i]);
        pthread_mutex_lock(&condition.mutex[(i + 1) % num_phsp]);
    
    }

    
    if(lock > 0){
         if(lock == i){
                    pthread_mutex_lock(&condition.mutex[(i + 1) % num_phsp]);
                } else{
            pthread_mutex_lock(&condition.mutex[i]);     
        }
    }

    return (clock() - start) * 1000 / CLOCKS_PER_SEC;
}



void putdown(int i){
    
    condition.status[i] = THINKING;//when release the chopstick, set state as thinking

    pthread_mutex_unlock(&condition.mutex[i]); //unluck the left and right mutex
    pthread_mutex_unlock(&condition.mutex[(i + 1) % num_phsp]);
    test((i + 1) % num_phsp);//test the conditions
    test((i + num_phsp - 1) % num_phsp);
  }


void* philosopher(void* arg){
	int id = *((int *)arg);
	int time = 0;
	int dining_time ;
	int hungry=0;
    printf("Philosopher %d is thinking. \n",id ); //initially all philosophers thinking

    while(time < num){
        
        usleep((useconds_t)(set_time(max_think, min_think) * 1000));
        
        hungry=hungry+ Pickup(id);//pickup the chopstick if left and right one is empty

        dining_time = set_time(max_dine, min_dine); //calculate dining time 
        
        clock_t start = clock();
        clock_t elapsed;
        
        int msec;
        
        while(msec < dining_time){

            elapsed = clock() - start;
            msec = elapsed * 1000 / CLOCKS_PER_SEC;

        }

        putdown(id);//release the chopsticks

        time = time + 1;
    }

    printf("Philosopher %d waits  %d time in total for hungry time \n",id , hungry);//when time is completed, exit the philosopher
    printf("Philosopher %d finished its %d dining time \n",id , dining_time);//when time is completed, exit the philosopher

    pthread_exit(NULL);

}

int main(int argc, char *argv[]){
  int i;

	 num_phsp = atoi(argv[1]);//take arguments from console
	if(num_phsp%2==0 || num_phsp>27){
	    printf("Error: The number of philosopher is bigger than 27 or it is not an odd number.");
	    exit(0);
	}
	
	 min_think = atoi(argv[2]);
	 max_think = atoi(argv[3]);
	 min_dine = atoi(argv[4]);
	 max_dine = atoi(argv[5]);
	 dst = argv[6];
	 num = atoi(argv[7]);
	if(min_think<1 || min_dine<1){
	printf("Please enter a value bigger than or equal to 1 milisecond");
	exit(0);
	}
	if(max_think>60000 || max_dine > 60000){
        printf("Please enter a value bigger than or equal to 60 second");
        exit(0);
	}
	
	condition.mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * num_phsp);//arrange the size of mutex
	condition.s = (pthread_cond_t *)malloc(sizeof(pthread_cond_t) * num_phsp);
	condition.status = (enum state *)malloc(sizeof(enum state) * num_phsp);
	
	initialization(num_phsp);//initialize to thinking state
	
	pthread_t tid[num_phsp];//crate thread ids
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
 	int plid[num_phsp];

     for(int i = 0; i < num_phsp; i++){
        plid[i] = i;
        pthread_create(&tid[i], &attr, philosopher, (void *)&plid[i]);//create theread with function
    }
     
  
    for(int i = 0; i < num_phsp; i++){
        pthread_join(tid[i], NULL);//whenn all of them finished join
    }

    
    exit(0);
}

