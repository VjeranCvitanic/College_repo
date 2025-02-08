#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/times.h>

#define PRINT_LVL 2

#define PRINT(...) PRINT__(1, __VA_ARGS__)
#define PRINT__(LVL, ...) if(LVL <= PRINT_LVL)printf(__VA_ARGS__)

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


void* simulator_function(void*);
int parse_input();
void controller(int N, pthread_t* threads);
void handle_sigint(int);
void print_task(task my_task);
void delay_until(int t, int* clock_);
void print_stats(stats Stats);
void print_all_stats();
int gettime();

//volatile int simulation = 1;
volatile int* state = NULL;
volatile int clock_main = 0;
volatile int clock_init = 0;
int N = 0;
stats* AllStats = NULL;
pthread_t* threads = NULL;

const task input[] = { //{.T, .t0, .C, .x} x=1 stvarni ulaz, x=0 popuna (LAB2)
//ovaj redak svake sekunde
{ 1000, 0, 30, 1}, {1000, 400, 30, 1}, {1000, 700, 30, 1}, //a1-a3

//samo jedan od ovih pet redaka svake sekunde
{ 5000,  100, 50, 1}, {5000,  500, 50, 1}, {5000,  800, 50, 1}, //b1-b3
{ 5000, 1100, 50, 1}, {5000, 1500, 50, 1}, {5000, 1800, 50, 1}, //b4-b6
{ 5000, 2100, 50, 1}, {5000, 2500, 50, 1}, {5000, 2800, 50, 1}, //b7-b9
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
{20000, 19900, 150, 0}   //c20-samo popunjavanje tablice
};


/*
const task input[] = { //{.T, .t0, .C, .x} x=1 stvarni ulaz, x=0 popuna (LAB2)
//ovaj redak svake sekunde
{ 1000, 0, 30, 1}, {1000, 400, 30, 1}, {1000, 700, 30, 1}, {20000,   900,  50, 1},  //c1
{20000,  1900, 150, 1},  //c2
{20000,  2900,  50, 1},  //c3
{20000,  3900, 150, 1},  //c4
{20000,  4900,  50, 1},  //c5
{20000,  5900, 150, 1},  //c6
{20000,  6900,  50, 1}};  //c7 //a1-a3
*/

int main()
{
	N = parse_input();
	
	state = (int*)calloc(N, sizeof(int));				// Structure for simulating input data
	threads = (pthread_t*)malloc(N*sizeof(pthread_t));		// Array of thread_ids
	AllStats = (stats*)malloc(N*sizeof(stats));			// Array of statistics for every input thread
	
	PRINT("Creating %d threads!\n", N);
	
	sleep(1);
	
	for(int i = 0; i < N; i++)
	{
		int* thread_index = (int*)malloc(sizeof(int));
		*thread_index = i;
		pthread_create(&threads[*thread_index], NULL, simulator_function, (void*)thread_index);

		PRINT__(5, "Thread %d created (tid: %ld)\n", *thread_index, threads[*thread_index]);
	}

	PRINT__(5, "\n\nSTATE LIST:\n");
	for(int i = 0; i < N; i++)
	{
		PRINT__(5, "%d: %d\n", i, state[i]);
	}

	PRINT__(5, "\nEND\n");
	
	controller(N, threads);

	return 0;
}

// Function for input threads
void* simulator_function(void* arg)
{
	int t = 0;
	int t_change_start = 0;
	int t_change_end = 0;
	int i = *(int*)arg;
	
	stats* MyStats = &AllStats[i];
	
	free(arg);
	
	PRINT__(5, "Thread %ld here (pid: %d)\n", pthread_self(), getpid());
	PRINT__(5, "My i: %d ", i);

	task my_task = input[i];
	
	if(my_task.x == 0)
		pthread_exit(NULL);
	
	t = my_task.t0;
	while(1)
	{
		delay_until(t, NULL);					// Delay until it is time for new input data to come
		state[i] = 1;						// Input
		t_change_start = clock_main;
		PRINT("%d\tInput %d: New input\n", t_change_start, i);
		
		while(state[i] != 0 && t + my_task.T > clock_main)	// Continue until data has been processed or until it is time for new data to be processed
		{
			PRINT__(3, "clock: %d", clock_main);
			delay_until(clock_main + 10, NULL);
		}
					
		t_change_end = clock_main;
		
		if(state[i] == 0)					// Successfully processed data
		{
			int reaction_time = (t_change_end - t_change_start) - my_task.C;
			MyStats->num_of_successfull_cases += 1;
			MyStats->reaction_time_sum += reaction_time;
			if(reaction_time > MyStats->max_reaction_time)
				MyStats->max_reaction_time = reaction_time;
			PRINT__(2, "%d\tWork on input %d\tDONE reaction time = %d\n", clock_main, i, reaction_time);
		}
		else							// Unsuccessfully processed data
		{
			MyStats->num_of_unsuccessfull_cases += 1;
			PRINT__(2, "%d\tWork on input %d NOT FINISHED\n", clock_main, i);
		}
		
		t += my_task.T;
	}

	pthread_exit(NULL);
	return NULL;
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

// Main thread function
void controller(int N, pthread_t* threads)
{	
	signal(SIGINT, handle_sigint);
	
	clock_init  = gettime();

	while(1)
	{
		for(int i = 0; i < N; i++)
		{
			if(state[i] == 1)
			{	
				struct timespec request = {0, input[i].C * 1000000};
				PRINT__(2, "%d\tWorking on input %d\n", clock_main, i);
				
				delay_until(clock_main + input[i].C, (void*)&clock_main);	// Update clock_main continuously, but wait until input[i].C amount of time passes
				
				//clock_main = gettime() - clock_init;
				state[i] = 0;							// Data processed
				PRINT__(2, "%d\tWork on input %d\tDONE\n", clock_main, i);
			}
			clock_main = gettime() - clock_init;					// Update clock_main
		}
	}
}

// Stop the program
void handle_sigint(int sig)
{
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

void delay_until(int t, int* clock_)
{
	int ms = 1;							// 5 ms
	if(clock_ == NULL)
	{							
		struct timespec request = {0, ms * 1000000};		// 1 000 000 ns = 1 ms
		while(clock_main < t)
		{
			nanosleep(&request, NULL);
		}
	}
	else
	{
		struct timespec request = {0, ms * 1000000};		// 1 000 000 ns = 1 ms
		while(*clock_ < t)
		{	
			nanosleep(&request, NULL);
			*clock_ += ms;
		}
	}
}

void print_stats(stats Stats)
{
	float avg_success = Stats.num_of_successfull_cases ? (float)Stats.reaction_time_sum/Stats.num_of_successfull_cases : 0;
	
	PRINT("Successfull cases:%d\tAVG time successfull cases: %f ms\tMax reaction time:%d ms\tNumber of unsuccessfull cases %d\n", Stats.num_of_successfull_cases, avg_success, Stats.max_reaction_time, Stats.num_of_unsuccessfull_cases);	
}

void print_all_stats()
{
	stats CombinedStats = {.num_of_successfull_cases = 0, .num_of_unsuccessfull_cases = 0, .reaction_time_sum = 0, .max_reaction_time = 0};
	PRINT("\n\nTime interval: [0 - %d] ms\n\n", clock_main);
	
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

int gettime()
{
	struct timeval my_timeval = { .tv_sec = 0, .tv_usec = 0 };
	gettimeofday(&my_timeval, NULL);
	return my_timeval.tv_sec * 1000 + (int)(my_timeval.tv_usec/1000);
}
