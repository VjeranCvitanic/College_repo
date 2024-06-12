#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <vector>
#include "mpi.h"
#include "board.h"
#include <iostream>
#include <chrono>
using namespace std::chrono;

// 0 - empty
// 1 - player 1
// -1 - player 2

#define MAX_BUFFER_SIZE 50
#define WORK_REQ 0
#define WORK_RSP 1
#define WORK_RES 2

const int DEPTH = 9;	// default dubina stabla


typedef struct _context{
    Board board;
    int parent_index;
    bool is_over;
    int depth;
    double factor = 1.0;
}context;

typedef struct _result{
    int parent_index;
    float value;
}result;

void create_tasks(Board board, int player_number, int* last_col, int* last_row);
void print_board(Board board);
double Evaluate(Board Current, int LastMover, int iLastCol, int iDepth);
int bot_master();
int bot_worker();

extern int size = 0;


int main(int argc, char** argv)
{
    int rank = 0;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0)
    {
        printf("Size: %d\n", size);
        bot_master();
    }
    else
    {
        bot_worker();
    }

    MPI_Finalize();
}

int bot_master()
{
    Board b;

    /*b.field[0][0] = HUMAN;
    b.field[0][5] = HUMAN;
    b.field[1][5] = HUMAN;
    b.field[0][3] = HUMAN;
    b.field[1][3] = HUMAN;
    b.field[0][1] = CPU;
    b.field[0][2] = CPU;
    b.field[0][4] = CPU;*/

    b.Load("ploca.txt");
    
    int last_move_row = 0, last_move_col = 0;
    int player_to_move = CPU;
    b.LastMover = HUMAN;
    int res = 0;

    printf("Welcome to game of Connect 4!\n");
    print_board(b);
    
    printf("Calculating...\n");

    auto start_time = std::chrono::high_resolution_clock::now();
    create_tasks(b, player_to_move, &last_move_col, &last_move_row);
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end_time - start_time);
 
    std::cout << duration.count() << " ms" << std::endl;

    //print_board(b);

    //printf("%d\n", b[0][4]);
    b.Move(last_move_col);
    //printf("%d\n", b[0][4]);
    print_board(b);


    printf("Played move is: Col: %d\n", last_move_col);

    if(b.GameEnd(last_move_col))
        printf("End\n");

    return 0;
}

int bot_worker()
{
    //printf("HERE\n");
    fflush(stdout);
    context c;
    result r;
    MPI_Status status;

    while(true)
    {
        //printf("BEFORE SEND\n");
        //fflush(stdout);
        MPI_Send(&c, 0, MPI_CHAR, 0, WORK_REQ, MPI_COMM_WORLD);
        //printf("AFTER SEND\n");
        //fflush(stdout);
        MPI_Recv(&c, sizeof(c), MPI_CHAR, 0, WORK_RSP, MPI_COMM_WORLD, &status);
        //printf("AFTER RECV\n");

        if(c.is_over)
        {
            //printf("WORKER END \n\n\n");
            //fflush(stdout);
            return 0;
        }
        //printf("depth: %d\n", c.depth);
        r.value = c.factor * Evaluate(c.board, c.board.LastMover,c.board.lastcol, DEPTH - c.depth);
        //printf("AFTER BP\n");
        fflush(stdout);
        r.parent_index = c.parent_index;

        MPI_Send(&r, sizeof(r), MPI_CHAR, 0, WORK_RES, MPI_COMM_WORLD);
        //printf("After send 2\n");
        fflush(stdout);
    }
}

int DEPTH_ = 3;
void create_tasks(Board board, int player_number, int* last_col, int* last_row)
{
    float ret_val = 0;
    int col = 0, row = 0;
    //int depth = DEPTH;
    int n = 1;
    std::vector<context> master_tasks;
    std::vector<context> bot_tasks;
    result r;
    int jobs_waiting = 0;
    MPI_Status status;
    int max_col = 0;
    double scores[7]{ 0 };

    {
        context c;
        c.board = board;
        c.depth = 0;
        c.is_over = false;

        master_tasks.push_back(c);
    }

    while(master_tasks.size())
    {
        auto current = master_tasks.back();
        master_tasks.pop_back();

        std::vector<context> temp_ctxs;

        for(int i = 0; i < 7; i++)
        {
            context next = current;

            if(next.depth == 0) next.parent_index = i;

            if(next.board.MoveLegal(i) == false) continue;
            
            next.depth++;
            next.board.Move(i);
            temp_ctxs.push_back(next);
        }

        for(auto& ctx : temp_ctxs)
        {
            ctx.factor *= 1.0 / temp_ctxs.size();
        }

        for(auto& ctx : temp_ctxs)
        {
            if(ctx.board.GameEnd(ctx.board.lastcol))
            {
                scores[ctx.parent_index] += ctx.factor * (ctx.board.LastMover == HUMAN ? -1 : 1);
            }
            else if(ctx.depth >= DEPTH_)   bot_tasks.push_back(ctx);
            else                            master_tasks.push_back(ctx);
        }
    }

    printf("size_of_worker_tasks: %d\n", bot_tasks.size());
    fflush(stdout);

    //printf("Ante");
    //fflush(stdout);

    while(bot_tasks.size() || jobs_waiting)
    {
        //printf("MASTER bot_tasks_size: %u, jobs_waiting: %d\n", bot_tasks.size(), jobs_waiting);
        MPI_Recv(&r, sizeof(r), MPI_CHAR, MPI_ANY_SOURCE, bot_tasks.size() ? MPI_ANY_TAG : WORK_RES, MPI_COMM_WORLD, &status);

        if(status.MPI_TAG == WORK_REQ)
        {
            jobs_waiting++;
            //fflush(stdout);
            MPI_Send(&bot_tasks.back(), sizeof(bot_tasks.back()), MPI_CHAR, status.MPI_SOURCE, WORK_RSP, MPI_COMM_WORLD);
            bot_tasks.pop_back();
        }
        else if(status.MPI_TAG == WORK_RES)
        {
            //printf("%d %d\n", jobs_waiting, bot_tasks.size());
            //fflush(stdout);
            jobs_waiting--;
            // if(r.value > max)
            // {
            //     max = r.value;
            //     // max_col = r.col; //r.col
            // }
            scores[r.parent_index] += r.value;

            // printf("col: %d, value: %f, parent index: %d\n", r.col, r.value, r.parent_index);
        }
    }

    //printf("MASTER END \n\n\n");
    context c;
    c.is_over = true;
    for(int i = 0; i < size - 1; i++)
    {
        MPI_Recv(&c, 0, MPI_CHAR, MPI_ANY_SOURCE, WORK_REQ, MPI_COMM_WORLD, &status);
        //printf("Gasim %d", i); fflush(stdout);
        c.is_over = true;
        MPI_Send(&c, sizeof(context), MPI_CHAR, status.MPI_SOURCE, WORK_RSP, MPI_COMM_WORLD);
    }
    //printf("MASTER END 2\n\n\n");
    //fflush(stdout);

    int index = 0;
    for(int i = 0; i < 7; i++)
    {
        //printf("%lf ", scores[i]);
        if(scores[i] > scores[index]) index = i;
    }


    *last_col = index;
}

void print_board(Board board)
{
    for(int i = 5; i >= 0; i--)
    {
        printf("\n");
        for(int k = 0; k < 7 - 2; k += 2)
            printf("----------");
        printf("\n");
        for(int j = 0; j < 7; j++)
        {
            printf(" |%d ", board[i][j]);
        }
        printf("|");
    }
    printf("\n");
        for(int k = 0; k < 7 - 2; k += 2)
            printf("---------");
        printf("\n");
    fflush(stdout);
}

// rekurzivna funkcija: ispituje sve moguce poteze i vraca ocjenu dobivenog stanja ploce
// Current: trenutno stanje ploce
// LastMover: HUMAN ili CPU
// iLastCol: stupac prethodnog poteza
// iDepth: dubina se smanjuje do 0
double Evaluate(Board Current, int LastMover, int iLastCol, int iDepth)
{
	double dResult, dTotal;
	int NewMover;
	bool bAllLose = true, bAllWin = true;
	int iMoves;
	
	if(Current.GameEnd(iLastCol))	// igra gotova?
		if(LastMover == CPU)
			return 1;	// pobjeda
		else //if(LastMover == HUMAN)
			return -1;	// poraz
	// nije gotovo, idemo u sljedecu razinu
	if(iDepth == 0)
		return 0;	// a mozda i ne... :)
	iDepth--;
	if(LastMover == CPU)	// tko je na potezu
		NewMover = HUMAN;
	else
		NewMover = CPU;
	dTotal = 0;
	iMoves = 0;	// broj mogucih poteza u ovoj razini
	for(int iCol=0; iCol<Current.Columns(); iCol++)
	{	if(Current.MoveLegal(iCol))	// jel moze u stupac iCol
		{	iMoves++;
			Current.Move(iCol, NewMover);
			dResult = Evaluate(Current, NewMover, iCol, iDepth);
			Current.UndoMove(iCol);
			if(dResult > -1)
				bAllLose = false;
			if(dResult != 1)
				bAllWin = false;
			if(dResult == 1 && NewMover == CPU)
				return 1;	// ako svojim potezom mogu doci do pobjede (pravilo 1)
			if(dResult == -1 && NewMover == HUMAN)
				return -1;	// ako protivnik moze potezom doci do pobjede (pravilo 2)
			dTotal += dResult;
		}
	}
	if(bAllWin == true)	// ispitivanje za pravilo 3.
		return 1;
	if(bAllLose == true)
		return -1;
	dTotal /= iMoves;	// dijelimo ocjenu s brojem mogucih poteza iz zadanog stanja
	return dTotal;
}
