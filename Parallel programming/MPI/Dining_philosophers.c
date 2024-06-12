#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 32
#define MAX_SLEEP_TIME 9
#define MIN_SLEEP_TIME 4
#define GIVE_FORK "Give me fork"
#define TAKE_FORK "Take the fork"

typedef struct Fork_ {
    int do_i_have; // 1 - yes, 0 - no
    bool washed;
}Fork;

typedef struct MyState_ {
    int rank;
    int size;
    int mode; // 0 - think, 1 - get forks, 2 - eat
    int my_requests[2];
    int others_requests[2];
    Fork fork[2];
}MyState;

MyState think(MyState);
MyState note_request(MyState, char*, MPI_Status*);
MyState get_forks(MyState);
MyState listen(MyState);
MyState eat(MyState);
int correct_num(MyState, int);
MyState process_getting_fork(MyState);
MyState setup_forks(MyState);
char* bool_to_string(bool);
void print_tabs(MyState);
MyState process_requests(MyState);

int main(int argc, char** argv)
{
    int rank, size;
    bool done = false;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MyState mystate = {.rank = rank, .size = size, .mode = 0, .others_requests = {0, 0}, .my_requests = {0, 0}, .fork = {{0, false},{0, false}}};

    srand(time(NULL) + rank);

    mystate = setup_forks(mystate);
    print_tabs(mystate);
    printf("My id: %d(size: %d), my forks: %d %d\n", mystate.rank, mystate.size, mystate.fork[0], mystate.fork[1]);

    while (!done)
    {
        mystate = think(mystate);
        print_tabs(mystate);
        printf("Philosopher %d done thinking, trying to get forks (curr: %d,%d)\n", mystate.rank, mystate.fork[0].do_i_have, mystate.fork[1].do_i_have);
        fflush(stdout);
        mystate = get_forks(mystate);
        fflush(stdout);
        mystate = eat(mystate);
        print_tabs(mystate);
        printf("Philosopher %d done eating\n", mystate.rank);
        mystate = process_requests(mystate);
        fflush(stdout);
    }

    MPI_Finalize();
}

MyState think(MyState mystate)
{
    int flag = 0;
    MPI_Status Status;
    int rand_num = rand() % (MAX_SLEEP_TIME - MIN_SLEEP_TIME) + MIN_SLEEP_TIME;
    char* buffer = "";

    mystate.mode = 0;
    mystate.my_requests[0] = 0;
    mystate.my_requests[1] = 0;
    print_tabs(mystate);
    printf("Philosopher %d thinking...for %d s\n", mystate.rank, rand_num);

    while (rand_num > 0)
    {
        mystate = listen(mystate);
        fflush(stdout);

        sleep(1);
        rand_num--;
    }

    return mystate;
}

MyState listen(MyState mystate)
{
    int flag = 0;
    MPI_Status Status;
    char buffer[MAX_BUFFER_SIZE] = "";

    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &Status);
    if (flag == 1)
    {
        MPI_Recv(buffer, MAX_BUFFER_SIZE, MPI_CHAR, Status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
        print_tabs(mystate);
        printf("Philosopher %d(%d,%d) received a msg (%s) from %d\n", mystate.rank, mystate.fork[0].do_i_have, mystate.fork[1].do_i_have, buffer, Status.MPI_SOURCE);
        fflush(stdout);
        mystate = note_request(mystate, buffer, &Status);
        fflush(stdout);
        return listen(mystate);
    }
    mystate = process_requests(mystate);
    return mystate;
}

MyState note_request(MyState mystate, char* buffer, MPI_Status* Status)
{
    if (strcmp(buffer, GIVE_FORK) == 0)
    {
        if (Status->MPI_SOURCE == correct_num(mystate, 0) && mystate.others_requests[1] == 0)
        {
            mystate.others_requests[1] = 1;
        }
        else if (Status->MPI_SOURCE == correct_num(mystate, 1) && mystate.others_requests[0] == 0)
        {
            mystate.others_requests[0] = 1;
        }
        else
        {
            print_tabs(mystate);
            printf("-----Wrong sender!(curr: %d, sender: %d)-----\n", mystate.rank, Status->MPI_SOURCE);
        }
    }
    else if (strcmp(buffer, TAKE_FORK) == 0)
    {
        if (Status->MPI_SOURCE == correct_num(mystate, 0) && mystate.fork[1].do_i_have == 0)
        {
            mystate.fork[1].do_i_have = 1;
            mystate.fork[1].washed = true;
        }
        else if (Status->MPI_SOURCE == correct_num(mystate, 1) && mystate.fork[0].do_i_have == 0)
        {
            mystate.fork[0].do_i_have = 1;
            mystate.fork[0].washed = true;
        }
        else
        {
            print_tabs(mystate);
            printf("-----Wrong sender!(curr: %d, sender: %d)-----\n", mystate.rank, Status->MPI_SOURCE);
        }
    }
    else
    {
        print_tabs(mystate);
        printf("-----Wrong message!-----\n");
    }

    return mystate;
}

MyState process_requests(MyState mystate)
{
    if(mystate.others_requests[0] == 1)
    {
        if(mystate.fork[0].do_i_have == 1 && mystate.fork[0].washed == false)
        {
            print_tabs(mystate);
            printf("Philosopher %d(%d,%d)(%s, %s) giving fork to %d\n", mystate.rank, mystate.fork[0].do_i_have, mystate.fork[1].do_i_have, bool_to_string(mystate.fork[0].washed), bool_to_string(mystate.fork[1].washed), correct_num(mystate, 1));
            mystate.fork[0].do_i_have = 0;
            mystate.others_requests[0] = 0;
            MPI_Request request;
            MPI_Isend(TAKE_FORK, strlen(TAKE_FORK), MPI_CHAR, correct_num(mystate, 1), 99, MPI_COMM_WORLD, &request);
        }
    }
    if(mystate.others_requests[1] == 1)
    {
        if(mystate.fork[1].do_i_have == 1 && mystate.fork[1].washed == false)
        {
            print_tabs(mystate);
            printf("Philosopher %d(%d,%d)(%s, %s) giving fork to %d\n", mystate.rank, mystate.fork[0].do_i_have, mystate.fork[1].do_i_have, bool_to_string(mystate.fork[0].washed), bool_to_string(mystate.fork[1].washed), correct_num(mystate, 0));
            mystate.fork[1].do_i_have = 0;
            mystate.others_requests[1] = 0;
            MPI_Request request;
            MPI_Isend(TAKE_FORK, strlen(TAKE_FORK), MPI_CHAR, correct_num(mystate, 0), 99, MPI_COMM_WORLD, &request);
        }
    }

    return mystate;
}

MyState get_forks(MyState mystate)
{
    mystate.mode = 1;
    while (mystate.fork[0].do_i_have * mystate.fork[1].do_i_have == 0)
    {
        if (mystate.fork[0].do_i_have == 0 && mystate.my_requests[0] == 0)
        {
            int val = correct_num(mystate, 1);
            if(val != -1)
            {
                mystate.my_requests[0] = 1;
                MPI_Request request;
                print_tabs(mystate);
                printf("Philosopher %d (%d,%d) requesting fork from %d\n", mystate.rank, mystate.fork[0].do_i_have, mystate.fork[1].do_i_have, val);
                MPI_Isend(GIVE_FORK, strlen(GIVE_FORK), MPI_CHAR, val, 99, MPI_COMM_WORLD, &request);
            }
        }
        else if (mystate.fork[1].do_i_have == 0 && mystate.my_requests[1] == 0)
        {
            int val = correct_num(mystate, 0);
            if(val != -1)
            {
                mystate.my_requests[1] = 1;
                MPI_Request request;
                print_tabs(mystate);
                printf("Philosopher %d (%d,%d) requesting fork from %d\n", mystate.rank, mystate.fork[0].do_i_have, mystate.fork[1].do_i_have, val);
                MPI_Isend(GIVE_FORK, strlen(GIVE_FORK), MPI_CHAR, val, 99, MPI_COMM_WORLD, &request);
            }   
        }
        fflush(stdout);
        mystate = listen(mystate);
        sleep(1);
    }

    print_tabs(mystate);
    printf("Philosopher %d got both forks!\n", mystate.rank);
    return mystate;
}

MyState eat(MyState mystate)
{
    int rand_num = rand() % (MAX_SLEEP_TIME - MIN_SLEEP_TIME) + MIN_SLEEP_TIME;
    char* buffer = "";

    mystate.mode = 2;

    print_tabs(mystate);
    printf("Philosopher %d eating...for %d s\n", mystate.rank, rand_num);
    fflush(stdout);
    mystate.fork[0].washed = false;
    mystate.fork[1].washed = false;

    while (rand_num > 0)
    {
        sleep(1);
        rand_num--;
    }

    return mystate;
}

int correct_num(MyState mystate, int direction)
{
    int val = 0;
    if (direction == 0)
    {
        if (mystate.rank == 0)
        {
            val = mystate.size - 1;
        }
        else
            val = mystate.rank - 1;
    }
    else
    {
        if (mystate.rank == mystate.size - 1)
        {
            val = 0;
        }
        else
            val = mystate.rank + 1;
    }

    if(val != mystate.rank)
    {
        return val;
    }
    else
    {
        return -1;
    }
}

MyState setup_forks(MyState mystate)
{
    if (mystate.rank == 0)
    {
        mystate.fork[0].do_i_have = 1;
        mystate.fork[1].do_i_have = 1;
    }
    else if (mystate.rank == mystate.size - 1)
    {
        mystate.fork[0].do_i_have = 0;
        mystate.fork[1].do_i_have = 0;
    }
    else
    {
        mystate.fork[0].do_i_have = 1;
        mystate.fork[1].do_i_have = 0;
    }

    return mystate;
}

char* bool_to_string(bool x)
{
    if(x)
        return "washed";
    return "dirty";
}

void print_tabs(MyState mystate)
{
    for(int i = 0; i < mystate.rank; i++)
        printf("\t");
}