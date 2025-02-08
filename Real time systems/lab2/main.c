#include <stdio.h>
#include <pthread.h>
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

#define LAST_N_EXEC 10

typedef struct _task {
	int T;
	int t0;
	int C;
	int x;
}task;

typedef struct _task_add {
	const task* task;
	bool second_p;
	bool recent_exec[LAST_N_EXEC];
	int i;
	int index;
}task_add;

typedef struct _stats {
	int num_of_successfull_cases;
	int num_of_unsuccessfull_cases;
	int reaction_time_sum;
	int max_reaction_time;
	int num_of_double_periods;
	int num_of_halts;
}stats;


void* simulator_function(void*);
int parse_input();
void controller(int N, pthread_t* threads);
void handle_sigint(int);
void print_task(task my_task);
void print_stats(stats Stats);
void print_all_stats();
int gettime();
int get_relative_time();
void do_work();
bool is_task_equal(task t1, task t2);
int choose_next_task();
bool halt_task();
void my_sleep(int ms);


volatile int* state = NULL;
volatile int clock_main = 0;
volatile int clock_init = 0;
int N = 0;
stats* AllStats = NULL;
pthread_t* threads = NULL;
task_add* running_task = NULL;					// What task is work currently being done on

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

int t = 0;										// task scheduling variables
int ind[] = {0, 0, 0};
int MAX_TIP[] = {3, 15, 20};
int zadA[] = {1, 2, 3};
int zadB[] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
int zadC[] = {19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38};
int* zad[3] = {zadA, zadB, zadC};
int red[] = {0, 1, -1, -1, 0, 1, -1, 0, 1, 2}; //za svaki prekid svakih 100 ms
//int red[] = {0, 1, -1, 0, 1, -1, 0, 1, 2, -1}; //za svaki prekid svakih 100 ms

int main()
{
	N = parse_input();

	running_task = (task_add*)malloc(sizeof(task_add));
	running_task->task = NULL;
	running_task->i = 0;
	running_task->index = -1;
	for(int k = 0; k < LAST_N_EXEC; k++)
	{
		running_task->recent_exec[k] = 0;
	}

	running_task->second_p = false;
	
	state = (int*)calloc(N, sizeof(int));				 // Structure for simulating input data
	threads = (pthread_t*)malloc(N*sizeof(pthread_t));				// Array of thread_ids
	AllStats = (stats*)malloc(N*sizeof(stats));				// Array of statistics for every input thread
	
	PRINT("Creating %d threads!\n", N);
	
	sleep(1);

	clock_init  = gettime();

	for(int i = 0; i < N; i++)
	{
		int* thread_index = (int*)malloc(sizeof(int));
		*thread_index = i;
		pthread_create(&threads[*thread_index], NULL, simulator_function, (void*)thread_index);
	}
	
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
		my_sleep(t - get_relative_time());
		state[i] = 1;						// Input
		t_change_start = get_relative_time();
		PRINT("%d\tInput %d: New input\n", t_change_start, i);
		
		while(state[i] != 0 && t + my_task.T > get_relative_time())	// Continue until data has been processed or until it is time for new data to be processed
		{
			PRINT__(3, "clock: %d", clock_main);
			my_sleep(10);
		}
					
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
	struct sigaction act;
    act.sa_handler = handle_sigint;
    act.sa_flags = SA_NODEFER;
    sigemptyset(& act.sa_mask);
    sigaction(SIGALRM, &act, NULL);
	sigaction(SIGINT, &act, NULL);

	struct sigevent sev;
	sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
	struct itimerspec tv;
    timer_t timerid;
	timer_create(CLOCK_MONOTONIC, &sev, &timerid);
    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_nsec = 100000000;  // when timer expires, reset to 100ms
    tv.it_value.tv_sec = 0;
    tv.it_value.tv_nsec = 10000;   // 10 ms == 10000 us

	timer_settime(timerid, 0, &tv, NULL);

	while(1);
}

bool is_task_equal(task t1, task t2)
{
	if(t1.C == t2.C && t1.T == t2.T && t1.t0 == t2.t0)
		return true;
	return false;
}

int choose_next_task()
{
	int next = 0;
	int tip = red[t];
	t = (t+1)%10;
	if(tip != -1)
	{
		next = zad[tip][ind[tip]];
		ind[tip] = (ind[tip] + 1) % MAX_TIP[tip];
	}	
	return next - 1;
}

bool halt_task()
{
	if(running_task->second_p)
		return true;
	bool flag = false;
	for(int i = 0; i < LAST_N_EXEC; i++)
	{
		flag = flag || running_task->recent_exec[i];
	}

	PRINT("FLAG: %s\n\n", flag ? "true" : "false");

	return flag;
}

void do_work()
{
	//PRINT("STARTING WORK\n");
	while(1)
	{
		if(running_task->task != NULL)
		{
			if(halt_task() == false)
			{
				PRINT("DOUBLE PERIOD %d\n\n", running_task->index);
				running_task->second_p = true;
				running_task->recent_exec[running_task->i] = true;
				running_task->i = (running_task->i + 1) % LAST_N_EXEC;
				AllStats[running_task->index].num_of_double_periods++;
				return;
			}
			else {
				AllStats[running_task->index].num_of_halts++;
				PRINT("HALT\n\n\n\n");
				//return;
			}
		}

		int task = choose_next_task();
		/*if(is_task_equal(running_task->task, input[task]))
			return;*/
		running_task->task = &input[task];
		running_task->index = task;

		if(task == -1 || input[task].x == 0)
		{
			PRINT("Either period is empty or task is a filler\n");
			goto wait;
			//return;
		}
		if(state[running_task->index] == 0)
		{
			PRINT("THIS TASK ISNT READY YET (%d)\n\n", running_task->index);	
			goto wait;
			//return;
		}

		PRINT__(1, "%d\tWorking on input %d\n", get_relative_time(), running_task->index);
		
		my_sleep(input[running_task->index].C);

		state[running_task->index] = 0;							// Data processed
		PRINT__(2, "%d\tWork on input %d\tDONE\n", get_relative_time(), running_task->index);
		
wait:
		running_task->task = NULL;
		running_task->second_p = false;
		running_task->recent_exec[running_task->i] = false;
		running_task->i = (running_task->i + 1) % LAST_N_EXEC;

		return;
	}
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
			free(running_task);
			PRINT("\nGoodbye!\n");
			exit(0);
		case SIGALRM:
			//PRINT("\nTIMER: %d\n", get_relative_time());

			do_work();
			//PRINT("\nEND\n");
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
	
	PRINT("Successfull cases:%d\tAVG time successfull cases: %f ms\tMax reaction time:%d ms\tNumber of unsuccessfull cases %d\tNumber of double periods %d\tNumber of halts %d\n", Stats.num_of_successfull_cases, avg_success, Stats.max_reaction_time, Stats.num_of_unsuccessfull_cases, Stats.num_of_double_periods, Stats.num_of_halts);	
}

void print_all_stats()
{
	stats CombinedStats = {.num_of_successfull_cases = 0, .num_of_unsuccessfull_cases = 0, .reaction_time_sum = 0, .max_reaction_time = 0, .num_of_double_periods = 0, .num_of_halts = 0};
	PRINT("\n\nTime interval: [0 - %d] ms\n\n", get_relative_time());
	
	for(int i = 0; i < N; i++)
	{
		PRINT("Input %d: ", i);
		print_stats(AllStats[i]);
		CombinedStats.num_of_successfull_cases += AllStats[i].num_of_successfull_cases;
		CombinedStats.num_of_unsuccessfull_cases += AllStats[i].num_of_unsuccessfull_cases;
		CombinedStats.reaction_time_sum += AllStats[i].reaction_time_sum;
		CombinedStats.num_of_double_periods += AllStats[i].num_of_double_periods;
		CombinedStats.num_of_halts += AllStats[i].num_of_halts;
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
	struct timespec request = {ms / 1000, (ms % 1000) * 1000000};		// 1 000 000 ns = 1 ms

	nanosleep(&request, NULL);
}
