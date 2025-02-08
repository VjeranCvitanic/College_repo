#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/times.h>
#include <stdbool.h>
#include <sys/time.h>

#define PRINT_LVL 2

#define PRINT(...) PRINT__(1, __VA_ARGS__); fflush(stdout)
#define PRINT__(LVL, ...) if(LVL <= PRINT_LVL)printf(__VA_ARGS__); fflush(stdout);

typedef struct _task {
	int T;
	int t0;
	int C;
	int x;
}task;

typedef struct _stats {
	int num_of_successfull_cases;
	int num_of_unsuccessfull_cases;
	int reaction_time_sum;
	int max_reaction_time;
}stats;

typedef struct _task_index {
    task T;
    int index;
}task_index;


void* simulator_function(void*);
int parse_input();
void handle_sigint(int);
void print_task(task my_task);
void print_stats(stats Stats);
void print_all_stats();
int gettime();
int get_relative_time();
void my_sleep(int ms);				// radno cekanje
void my_sleep_2(int ms);			
void sleep_until(int t);
void sleep_until_2(int t);
void calc_10_ms();
void sleep_10_ms();
void* worker(void* Task);
int period_to_priority(int);


volatile int* state = NULL;
volatile int clock_init = 0;
int N = 0;
stats* AllStats = NULL;
pthread_t* threads = NULL;

volatile int num_to_sleep_10_ms;

const task input[] = { //{.T, .t0, .C, .x} x=1 stvarni ulaz, x=0 popuna (LAB2)
//ovaj redak svake sekunde
{ 1000, 0, 30, 1}, {1000, 400, 30, 1}, {1000, 700, 30, 1}, //a1-a3

//samo jedan od ovih pet redaka svake sekunde
{ 5000,  100, 50, 1}, {5000,  500, 50, 1}, {5000,  800, 50, 1}, //b1-b3
{ 5000, 1100, 50, 1}/*, {5000, 1500, 50, 1}, {5000, 1800, 50, 1}, //b4-b6
{ 5000, 2100, 50, 1}, {5000, 2500, 50, 1}
/*,{5000, 2800, 50, 1}, //b7-b9
{ 5000, 3100, 50, 1}, {5000, 3500, 50, 1}, {5000, 3800, 50, 1}, //b9-b12
{ 5000, 4100, 50, 1}, {5000, 4500, 50, 1}, {5000, 4800, 50, 1}, //b13-b15

//samo jedan od ovih 20 redaka svake sekunde
{20000,   900,  50, 1},  //c1
{20000,  1900, 150, 1},  //c2
{20000,  2900,  50, 1},  //c3
{20000,  3900, 150, 1},  //c4
{20000,  4900,  50, 1},  //c5
{20000,  5900, 150, 1},  //c6
{20000,  6900,  50, 1},  //c7
{20000,  7900, 150, 1},  //c8
{20000,  8900,  50, 1},  //c9
{20000,  9900, 150, 1},  //c10
{20000, 10900,  50, 1},  //c11
{20000, 11900, 150, 1},  //c12
{20000, 12900,  50, 1},  //c13
{20000, 13900, 150, 1},  //c14
{20000, 14900,  50, 1},  //c15
{20000, 15900, 150, 1},  //c16
{20000, 16900,  50, 1},  //c17
{20000, 17900, 150, 0},  //c18-samo popunjavanje tablice
{20000, 18900, 150, 0},  //c19-samo popunjavanje tablice
{20000, 19900, 150, 0}   //c20-samo popunjavanje tablice*/
};

int main()
{
	pthread_attr_t attr;
	struct sched_param priority;

	N = parse_input();
	
	state = (int*)calloc(N, sizeof(int));				 // Structure for simulating input data
	threads = (pthread_t*)malloc(N*sizeof(pthread_t));				// Array of thread_ids
	AllStats = (stats*)malloc(N*sizeof(stats));				// Array of statistics for every input thread

	struct sigaction act;
    act.sa_handler = handle_sigint;
	sigaction(SIGINT, &act, NULL);

    calc_10_ms();

	PRINT("\nNUM for 10 ms sleep: %d\n\n", num_to_sleep_10_ms);

	PRINT("Creating %d threads!\n", N);
		
	sleep(1);

	
	if(pthread_attr_init(&attr))
	{
		PRINT("ERROR in pthread_attr_init\n");
	}
	if(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED))
	{
		PRINT("ERROR in pthread_attr_setinheritsched\n");
	}

	if(pthread_attr_setschedpolicy(&attr, SCHED_FIFO))
	{
		PRINT("ERROR in pthread_attr_setschedpolicy\n");
	}

	clock_init  = gettime();

	for(int i = 0; i < N; i++)
	{
		int* thread_index = (int*)malloc(sizeof(int));
		*thread_index = i;
		priority.sched_priority = 100;//period_to_priority(input[i].T);
		if(pthread_attr_setschedparam(&attr, &priority))
		{
			PRINT("ERROR in pthread_attr_setschedparam\n");
		}
		if(pthread_create(&threads[*thread_index], &attr, simulator_function, (void*)thread_index))
		{
			PRINT("Error in pthread_create! (thread index: %d)\n", *thread_index);
			return 1;
		}
	}

    while(1);

	return 0;
}

void* worker(void* arg)
{
	task_index Task = *(task_index*)arg;
    int t = Task.T.t0;
    while(1)
    {
        sleep_until_2(t);			// real sleep

        while(state[Task.index] == 0);
		PRINT("%d\tWORKER started work on %d\n", get_relative_time(), Task.index);
        my_sleep(Task.T.C);		// radno cekanje
		PRINT("%d\tWORKER finished work on %d\n", get_relative_time(), Task.index);
        state[Task.index] = 0;

        t += Task.T.T;
    }
}

// Function for input threads
void* simulator_function(void* arg)
{
	int t = 0;
	int t_change_start = 0;
	int t_change_end = 0;
	int i = *(int*)arg;
	pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));

	pthread_attr_t attr;
	struct sched_param priority;
	
	stats* MyStats = &AllStats[i];
	
	free(arg);
	
	PRINT__(5, "Thread %ld here (pid: %d)\n", pthread_self(), getpid());
	PRINT__(5, "My i: %d ", i);

	task my_task = input[i];
    task_index T = {.T = my_task, .index = i};
	task_index* T_ptr = &T;
	
	if(my_task.x == 0)
		pthread_exit(NULL);

	if(pthread_attr_init(&attr))
	{
		PRINT("ERROR in pthread_attr_init\n");
	}
	if(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED))
	{
		PRINT("ERROR in pthread_attr_setinheritsched\n");
	}
	if(pthread_attr_setschedpolicy(&attr, SCHED_FIFO))
	{
		PRINT("ERROR in pthread_attr_setschedpolicy\n");
	}
	priority.sched_priority = period_to_priority(input[i].T);

	if(pthread_attr_setschedparam(&attr, &priority))
	{
		PRINT("ERROR in pthread_attr_setschedparam\n");
	}
	pthread_attr_setschedparam(&attr, &priority);

	if(pthread_create(thread, NULL, worker, (void*)T_ptr))
	{
		PRINT("ERROR in pthread_create worker: %d\n", i);
		exit(1);
	}
	
	t = my_task.t0;
	while(1)
	{
		sleep_until_2(t);
		state[i] = 1;						// Input
		t_change_start = get_relative_time();
		PRINT("%d\tInput %d: New input\n", t_change_start, i);
		
		while(state[i] != 0 && (t + my_task.T > get_relative_time()))	// Continue until data has been processed or until it is time for new data to be processed
		{
			my_sleep_2(10);
		}

		//PRINT("index: %d, t: %d, T: %d, curr_t: %d\n", i, t, my_task.T, get_relative_time());
					
		t_change_end = get_relative_time();
		
		if(state[i] == 0)					// Successfully processed data
		{
			int reaction_time = (t_change_end - t_change_start) - my_task.C;
			MyStats->num_of_successfull_cases += 1;
			MyStats->reaction_time_sum += reaction_time;
			if(reaction_time > MyStats->max_reaction_time)
				MyStats->max_reaction_time = reaction_time;
			PRINT__(2, "%d\tWork on input %d\tDONE reaction time = %d\n", get_relative_time(), i, reaction_time);
		}
		else							// Unsuccessfully processed data
		{
			MyStats->num_of_unsuccessfull_cases += 1;
			PRINT__(2, "%d\tWork on input %d NOT FINISHED\n", get_relative_time(), i);
		}
		
		t += my_task.T;
	}

	pthread_exit(NULL);
	return NULL;
}

void sleep_until(int t)
{
    int diff = t - get_relative_time();
    if(diff <= 0)
        return;
    my_sleep(diff);
}

void sleep_until_2(int t)
{
    int diff = t - get_relative_time();
    if(diff <= 0)
        return;
    my_sleep_2(diff);
}

int parse_input()
{
	PRINT("Size of input: %ld\t Size of task: %ld\n", sizeof(input), sizeof(task));
	PRINT("\n\nTASKS:\n");
	for(int i = 0; i < sizeof(input)/sizeof(task); i+=1)
	{
		print_task(input[i]);
	}
	
	return sizeof(input)/sizeof(task);
}

// Stop the program
void handle_sigint(int sig)
{
	switch(sig)
	{
		case SIGINT: 
			PRINT("Program received SIGINT: %d\n", sig);
			PRINT("Stopping simulation...\n");
			
			PRINT__(5, "Parent thread done, waiting for input simulator threads!\n");
			for(int i = 0; i < N; i++)
				pthread_cancel(threads[i]);
				
			PRINT("\n\n\nALL done\n\n");
			print_all_stats();
			
			free(threads);
			free((void*)state);
			PRINT("\nGoodbye!\n");
			exit(0);
	}
}

int gettime()
{
	struct timespec my_timeval = { .tv_sec = 0, .tv_nsec = 0 };
	clock_gettime(CLOCK_MONOTONIC_RAW, &my_timeval);
	return my_timeval.tv_sec * 1000 + (int)(my_timeval.tv_nsec/1000000);
}

int get_relative_time()
{
	return gettime() - clock_init;
}

void print_stats(stats Stats)
{
	float avg_success = Stats.num_of_successfull_cases ? (float)Stats.reaction_time_sum/Stats.num_of_successfull_cases : 0;
	
	PRINT("Successfull cases:%d\tAVG time successfull cases: %f ms\tMax reaction time:%d ms\tNumber of unsuccessfull cases %d\n", Stats.num_of_successfull_cases, avg_success, Stats.max_reaction_time, Stats.num_of_unsuccessfull_cases);	
}

void print_all_stats()
{
	stats CombinedStats = {.num_of_successfull_cases = 0, .num_of_unsuccessfull_cases = 0, .reaction_time_sum = 0, .max_reaction_time = 0};
	PRINT("\n\nTime interval: [0 - %d] ms\n\n", get_relative_time());
	
	for(int i = 0; i < N; i++)
	{
		PRINT("Input %d: ", i);
		print_stats(AllStats[i]);
		CombinedStats.num_of_successfull_cases += AllStats[i].num_of_successfull_cases;
		CombinedStats.num_of_unsuccessfull_cases += AllStats[i].num_of_unsuccessfull_cases;
		CombinedStats.reaction_time_sum += AllStats[i].reaction_time_sum;
		if(CombinedStats.max_reaction_time < AllStats[i].max_reaction_time)
		{
			CombinedStats.max_reaction_time = AllStats[i].max_reaction_time;
		}
	}
	
	PRINT("Combined stats:\n");
	print_stats(CombinedStats);
}

void print_task(task my_task)
{
	PRINT("{ %d\t", my_task.T);
	PRINT("%d\t", my_task.t0);
	PRINT("%d\t", my_task.C);
	PRINT("%d }\n", my_task.x);
}

void my_sleep(int ms)
{
	int target = get_relative_time() + ms;
	// More precise way of simulating work
	while(5 < target - get_relative_time())		// ~5 to minimize error -> [-5, +5]
	{
		sleep_10_ms();
	}
	/*for(int i = 0; i < ms/10; i++)
    {
        sleep_10_ms();
    }*/
}

void my_sleep_2(int ms)
{
	struct timespec request = {ms / 1000, (ms % 1000) * 1000000};		// 1 000 000 ns = 1 ms

	nanosleep(&request, NULL);
}

void sleep_10_ms()
{
    for(int i = 0; i < num_to_sleep_10_ms; i++)
    {
        asm volatile("" ::: "memory");
    } 
}

void calc_10_ms()
{
    num_to_sleep_10_ms = 1000000;
    bool run = true;
    int t0, t1;
    while(run)
    {
        t0 = gettime();
        sleep_10_ms();
        t1 = gettime();
        if(t1 - t0 >= 10)
            run = false;
        else
            num_to_sleep_10_ms *= 10;
    }
    num_to_sleep_10_ms = num_to_sleep_10_ms * 10 / (t1 - t0);
}

int period_to_priority(int period)
{
	switch (period) {
		case 1000:
			return 30;
		case 5000:
			return 20;
		case 20000:
			return 10;
		default:
			PRINT("ERROR in period_to_priority\n");
			return -1;
	}
}