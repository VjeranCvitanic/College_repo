#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <errno.h>

#define _OPEN_SYS_ITOA_EXT
#define _OPEN_SYS_LTOA_EXT

#define N 5

#define MAX_MESSAGE_SIZE sizeof(struct my_msgbuf)
#define MAX_TEXT_SIZE 10

//mtype
#define REQ 1
#define REP 2

#define PRINT(...) do{printf(__VA_ARGS__); \
    fflush(stdout); \
    }while(0)

struct my_msgbuf {
	long mtype;       /* message type, must be > 0 */
    int pid_src;
    int timestamp;
	char mtext[MAX_TEXT_SIZE];    /* message data */
};
typedef struct my_msgbuf* msg_pointer;

struct list_element;
typedef struct list_element* list_pointer;
struct list_element {
    msg_pointer request;
    list_pointer next;
};

struct process_data {
	int timestamp;
	int forks[2];		//{LEFT, RIGHT}
	int  left_fd[2];
    int right_fd[2];
	int* list_of_pids;
	list_pointer list_of_current_requests;
    int my_no;

	int my_pid;

	int msgq_right_neighbour;
	int msgq_left_neighbour;
};

void* create_children(void*, struct process_data*);
void* create_shared_memory(size_t size);
int prepare(int* list_of_pids, struct process_data* my_data, int* sync);
void print_list_of_pids(struct process_data* my_data);
void think(struct process_data* my_data_pointer);
void get_forks(struct process_data* my_data_pointer);
void eat(struct process_data* my_data_pointer);
int next_pid(int* list_of_pids, int curr);    // R
int prev_pid(int* list_of_pids, int curr);    // L
int no_from_pid(int* list_of_pids, int pid);
int receive_msg(struct process_data* my_data_pointer);
bool is_equal(msg_pointer request1, msg_pointer request2);
void update_timestamp(struct process_data* my_data_pointer, int new_timestamp);
void process_msg(struct process_data* my_data_pointer, msg_pointer buffer);
void send_reply_msgs(struct process_data* my_data_pointer);
bool check_if_first(struct process_data* my_data_pointer);

msg_pointer create_msg(long mtype, int pid_src, int timestamp, char mtext[MAX_TEXT_SIZE]);
void delete_msg(msg_pointer msg);

list_pointer list_create(msg_pointer request);
void list_add(list_pointer list, msg_pointer request);
void list_remove(list_pointer list, msg_pointer request);
list_pointer list_find(list_pointer first, long mtype, int pid);
void list_destroy(list_pointer list_element);
void list_destroy_all(list_pointer head);
void print_list(list_pointer first);
void print_msg(msg_pointer q);
void print_tabs(int n);

char* typeno_to_char(long type);

char* switch_fork(char a);

char* struct_to_char(msg_pointer message);
msg_pointer char_to_struct(char* message);

int main()
{
    struct list_element Head = {.request = NULL, .next = NULL};
    list_pointer first = &Head;

	struct process_data my_data = {.timestamp = 1, .forks = {0, 1}, .my_no = -1, .list_of_pids = NULL, .list_of_current_requests = first, .my_pid = 0, .right_fd = { 0 }, .left_fd = { 0 }};
	struct process_data* my_data_pointer = &my_data;
	void* shared_mem = NULL;
    void* shared_sync = NULL;

	shared_mem = create_shared_memory(2*N*sizeof(int));			// [{pid1, no1}, {pid2, no2}, ...]
    shared_sync = create_shared_memory(sizeof(int));
    memset(shared_sync, 0, sizeof(int));

    for(int i = 0; i < N; i++)
    {
        ((int*)shared_mem)[2*i] = 0;
        ((int*)shared_mem)[2*i + 1] = 0;
    }

	shared_mem = create_children(shared_mem, my_data_pointer);

	prepare(shared_mem, my_data_pointer, shared_sync);

	think(my_data_pointer);

	return 0;
}

void* create_shared_memory(size_t size) 
{
	int protection = PROT_READ | PROT_WRITE;
  	int visibility = MAP_SHARED | MAP_ANONYMOUS;

  	return mmap(NULL, size, protection, visibility, -1, 0);
}

void* create_children(void* shared_mem, struct process_data* my_data_pointer)
{
	int pid = getpid();
    int parent_pid = getpid();

    int all_fds_open[2*N][2] = { 0 };
	
    my_data_pointer->my_no = 1;

    ((int*)shared_mem)[0] = pid;

    // pipe for first and last process
    int ffd_1[2] = { 0 };
    int ffd_2[2] = { 0 };
    if(pipe(ffd_1) != 0)
        return NULL;
    if(pipe(ffd_2) != 0)
        return NULL;
    
    my_data_pointer->left_fd[0] = ffd_2[0]; // out
    my_data_pointer->left_fd[1] = ffd_1[1]; // in

    all_fds_open[0][0] = ffd_1[0];
    all_fds_open[0][1] = ffd_1[1];
    all_fds_open[1][0] = ffd_2[0];
    all_fds_open[1][1] = ffd_2[1];

	for(int i = 1; i < N; i++)
	{
        int fd_1[2] = { 0 };
        int fd_2[2] = { 0 };
        if(pipe(fd_1) != 0)
            return NULL;
        if(pipe(fd_2) != 0)
            return NULL;

        all_fds_open[2*(i)][0] = fd_1[0];
        all_fds_open[2*(i)][1] = fd_1[1];
        all_fds_open[2*(i) + 1][0] = fd_2[0];
        all_fds_open[2*(i) + 1][1] = fd_2[1];

		pid = fork();

		if(pid < 0)
			return NULL;
		else if(pid > 0)    // parent
		{
            my_data_pointer->right_fd[0] = fd_1[0]; // out
            my_data_pointer->right_fd[1] = fd_2[1]; // in
            break;
		}
        else // new child
        {
            ((int*)shared_mem)[2*i] = getpid();
            my_data_pointer->my_no = i + 1;
            my_data_pointer->left_fd[0] = fd_2[0]; // out
            my_data_pointer->left_fd[1] = fd_1[1]; // in
        }
	}

	if((((int*)shared_mem)[2*N-2] != 0) && (getpid() == ((int*)shared_mem)[2*N-2]))
    {
        my_data_pointer->right_fd[0] = ffd_1[0]; // out
        my_data_pointer->right_fd[1] = ffd_2[1]; // in
    }

    for(int i = 0; i < 2*N; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            int flag = 0;
            for(int k = 0; k < 2; k++)
            {
                if(my_data_pointer->left_fd[k] == all_fds_open[i][j])
                    flag = 1;
            }
            for(int k = 0; k < 2; k++)
            {
                if(my_data_pointer->right_fd[k] == all_fds_open[i][j])
                    flag = 1;
            }
            if(flag == 0)
                close(all_fds_open[i][j]);
        }
    }

	return shared_mem;
}

int prepare(int* list_of_pids, struct process_data* my_data, int* sync)
{	
	my_data->forks[0] = 0;
	my_data->forks[1] = 0;
	my_data->list_of_pids = list_of_pids;
	my_data->my_pid = getpid();
	my_data->msgq_right_neighbour = 0;
 	my_data->msgq_left_neighbour = 0;

	for(int i = 0; i < N; i++)
	{
		while(my_data->list_of_pids[2*i] == 0);			// to ensure that the parent process has put all pids in the list
	}

	for(int i = 0; i < N; i++)
	{
		if(my_data->list_of_pids[2 * i] == my_data->my_pid)
		{
            my_data->timestamp = my_data->my_no;
			my_data->list_of_pids[2 * i + 1] = my_data->my_no;
			for(int j = 0; j < N; j++)
			{
				while(my_data->list_of_pids[2*j+1] == 0);  // wait for everyone to do it
			}
		}
	}

    int flags = fcntl(my_data->left_fd[0], F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        return 1;
    }
    if (fcntl(my_data->left_fd[0], F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl set");
        return 1;
    }

    flags = fcntl(my_data->right_fd[0], F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        return 1;
    }
    if (fcntl(my_data->right_fd[0], F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl set");
        return 1;
    }

    if(my_data->my_pid == my_data->list_of_pids[0])
    {
        print_list_of_pids(my_data);
        getchar();
        *sync = 1;
    }
    else
    {
        while(*sync != 1);
    }

	for(int i = 0; i < N; i++)
	{
		if(my_data->my_pid == my_data->list_of_pids[2*i])
		{
			my_data->msgq_right_neighbour = next_pid(my_data->list_of_pids, 2*i);
 			my_data->msgq_left_neighbour = prev_pid(my_data->list_of_pids, 2*i);
		}
	}

	return 0;
}

void print_list_of_pids(struct process_data* my_data)
{
    PRINT("pids: \n");
	for(int i = 0; i < N; i++)
	{
        PRINT("%d ", my_data->list_of_pids[2 * i]);
	}

    PRINT("\nno: \n");
	for(int i = 0; i < N; i++)
	{
        PRINT("%d ", my_data->list_of_pids[2 * i + 1]);
	}
	
    PRINT("\n\n\n\n");
}

void think(struct process_data* my_data_pointer)
{	
	int upper_bound = 5, lower_bound = 2;
	srand(my_data_pointer->my_pid);
	int time_to_think = rand() % (upper_bound - lower_bound + 1) + lower_bound;
	//time_to_think=0;
    print_tabs(my_data_pointer->my_no);
    PRINT("%d thinking for %ds\n", my_data_pointer->my_no, time_to_think);

	do
	{	
		receive_msg(my_data_pointer);
		//primi poruke i obradi ih, azuriraj sat; obraditi poruke = stavi poruku u svoj red cekanja, salji nazad novu vrijednost sata
		time_to_think -= 1;
		sleep(1);
	}while(time_to_think > 0);

	return get_forks(my_data_pointer);
}

void get_forks(struct process_data* my_data_pointer)
{
    //print_tabs(my_data_pointer->my_no);
	//PRINT("%d trying to get forks(%d, %d)\n", my_data_pointer->my_no, my_data_pointer->forks[0], my_data_pointer->forks[1]);

    receive_msg(my_data_pointer);

    if(my_data_pointer->forks[0] == 0)
    {	
        print_tabs(my_data_pointer->my_no);
        PRINT("%d sending REQ message to %d (my timestamp: %d)\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, my_data_pointer->msgq_left_neighbour), my_data_pointer->timestamp);
        msg_pointer msg = create_msg(REQ, my_data_pointer->my_pid, my_data_pointer->timestamp, "L");
        char buff[MAX_MESSAGE_SIZE];
        strcpy(buff, struct_to_char(msg));
        write(my_data_pointer->left_fd[1], &buff, sizeof(buff));
        list_add(my_data_pointer->list_of_current_requests, msg);
    }
    if(my_data_pointer->forks[1] == 0)
    {	
        print_tabs(my_data_pointer->my_no);
        PRINT("%d sending REQ message to right %d (my timestamp: %d)\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, my_data_pointer->msgq_right_neighbour), my_data_pointer->timestamp);
        msg_pointer msg = create_msg(REQ, my_data_pointer->my_pid, my_data_pointer->timestamp, "R");
        char buff[MAX_MESSAGE_SIZE];
        strcpy(buff, struct_to_char(msg));
        write(my_data_pointer->right_fd[1], &buff, sizeof(buff));
        list_add(my_data_pointer->list_of_current_requests, msg);
    }

    while(!check_if_first(my_data_pointer))
        receive_msg(my_data_pointer);
	//šalji zahtjeve za vilicom/vilicama, stavi taj zahtjev u svoj red poruka
	//primi poruke i obradi ih, azuriraj sat; obraditi poruke = stavi poruku u svoj red cekanja, salji nazad  novu vrijednost sata
	//ako je moj zahtjev na pocetku reda i ako sam primio sve oznake vremena od ostalih procesa odradi kriticni odsjecak -> u ovom slucaju to je koristenje vilice
	//ako moj zahtjev nije na pocetku reda, ostavi vilicu, i cekaj
	// nakon izlaska iz K.O. posalji poruku izlaz
	//ako primim poruku izlaz, micem tu poruku iz reda

	return eat(my_data_pointer);
}

void eat(struct process_data* my_data_pointer)
{
	int upper_bound = 5, lower_bound = 2;
	srand(my_data_pointer->my_pid);
	int time_to_eat = rand() % (upper_bound - lower_bound + 1) + lower_bound;
    //time_to_eat=0;
    print_tabs(my_data_pointer->my_no);
	PRINT("%d eating for %ds\n", my_data_pointer->my_no, time_to_eat);

    /*PRINT("LIST: \n");
    print_list(my_data_pointer->list_of_current_requests);
    PRINT("\n\n\n");*/

    sleep(time_to_eat);

    my_data_pointer->forks[0] = 0;
    my_data_pointer->forks[1] = 0;
    send_reply_msgs(my_data_pointer);

	return think(my_data_pointer);
}

int next_pid(int* list_of_pids, int curr)
{
	if((curr + 2) >= 2*N)
	{
		return list_of_pids[0];
	}
	return list_of_pids[(curr + 2)];
}
int prev_pid(int* list_of_pids, int curr)
{
	if((curr - 2) < 0)
	{
		return list_of_pids[2*N-2];
	}
	return list_of_pids[(curr - 2)];
}

int receive_msg(struct process_data* my_data_pointer)
{	
	msg_pointer buffer = NULL;
	ssize_t size_of_msg = 0;
    char msg[MAX_MESSAGE_SIZE] = "";

	size_of_msg = read(my_data_pointer->left_fd[0], msg, MAX_MESSAGE_SIZE);
	if(size_of_msg != -1)
	{
        buffer = char_to_struct(msg);
        print_tabs(my_data_pointer->my_no);
		PRINT("%d receives a %s message from %d, my timestamp: %d, his timestamp: %d\n", my_data_pointer->my_no, typeno_to_char(buffer->mtype), no_from_pid(my_data_pointer->list_of_pids, buffer->pid_src), my_data_pointer->timestamp, buffer->timestamp);

        process_msg(my_data_pointer, buffer);
	}

    size_of_msg = read(my_data_pointer->right_fd[0], msg, MAX_MESSAGE_SIZE);
	if(size_of_msg != -1)
	{
        buffer = char_to_struct(msg);
        print_tabs(my_data_pointer->my_no);
		PRINT("%d receives a %s message from %d, my timestamp: %d, his timestamp: %d\n", my_data_pointer->my_no, typeno_to_char(buffer->mtype), no_from_pid(my_data_pointer->list_of_pids, buffer->pid_src), my_data_pointer->timestamp, buffer->timestamp);

        process_msg(my_data_pointer, buffer);
	}

    return 0;
}

char* switch_fork(char a)
{
    char* var = (char*)malloc(2*sizeof(char));
    var = "A";
    if(a == 'R')
        var = "L";
    else if(a == 'L')
        var = "R";
    else
        printf("Error, a: %s\n", a);

    return var;        
}


void process_msg(struct process_data* my_data_pointer, msg_pointer buffer)
{
    update_timestamp(my_data_pointer, buffer->timestamp);
    if(buffer->mtype == REQ)
    {
        list_pointer my_req = list_find(my_data_pointer->list_of_current_requests, REQ, my_data_pointer->my_pid);

        if ((my_req == NULL) || (my_req->request->timestamp >= buffer->timestamp))
        {
            print_tabs(my_data_pointer->my_no);
            PRINT("%d sending REP message to %d my timestamp: %d\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, buffer->pid_src), my_data_pointer->timestamp);
            msg_pointer msg = create_msg(REP, my_data_pointer->my_pid, my_data_pointer->timestamp, switch_fork(buffer->mtext[0]));
            char buff[MAX_MESSAGE_SIZE];
            strcpy(buff, struct_to_char(msg));
            if(buffer->pid_src == next_pid(my_data_pointer->list_of_pids, 2*(my_data_pointer->my_no-1)))
                write(my_data_pointer->right_fd[1], &buff, sizeof(buff));
            else
                write(my_data_pointer->left_fd[1], &buff, sizeof(buff));
            free(msg);
        }
        else
            list_add(my_data_pointer->list_of_current_requests, buffer);
    }
    else if(buffer->mtype == REP)
    {
		list_add(my_data_pointer->list_of_current_requests, buffer);
    }
    free(buffer);
}

msg_pointer create_msg(long mtype, int pid_src, int timestamp, char mtext[MAX_TEXT_SIZE])
{
    msg_pointer q = (msg_pointer)malloc(sizeof(struct my_msgbuf));
    if(!q)
    {
        PRINT("Error, Couldn't allocate msg!\n");
        return NULL;
    }

    q->mtype = mtype;
    q->pid_src = pid_src;
    q->timestamp = timestamp;
    strcpy(q->mtext, mtext);

    return q;
}

void delete_msg(msg_pointer msg)
{
    if(msg)
        free(msg);
}


list_pointer list_create(msg_pointer request)
{
    list_pointer q = (list_pointer)malloc(sizeof(struct list_element));
    if (!q)
    {
        PRINT("Error, Couldn't allocate memory for list_element!\n");
        return NULL;
    }

    q->request = create_msg(request->mtype, request->pid_src, request->timestamp, request->mtext);
    q->next = NULL;

    return q;
}

void list_destroy(list_pointer list_element)
{
    delete_msg(list_element->request);
    free(list_element);
}

void list_destroy_all(list_pointer head)
{
    list_pointer first = head;
    while(first && first->next)
    {
        list_pointer temp = first->next;
        first->next = first->next->next;
        list_destroy(temp);
    }
}

void list_add(list_pointer first, msg_pointer request)
{
    if(!first)
    {
        PRINT("Error, first = NULL\n");
        return;
    }

    list_pointer new = list_create(request);
    if(!new)
    {
        PRINT("Fail in list_add, new = NULL\n");
        return;
    }

    while (first->next && ((new->request->timestamp > first->next->request->timestamp) || ((new->request->timestamp == first->next->request->timestamp) && (new->request->pid_src > first->next->request->pid_src))))
    {
        first = first->next;
    }

    if(!first->next)
    {
        first->next = new;
        return;
    }

    new->next = first->next;
    first->next = new;
}

void list_remove(list_pointer first, msg_pointer request)
{
    if(!first)
    {
        PRINT("Error, first = NULL\n");
        return;
    } 

    list_pointer temp = first->next;
    list_pointer prev = first;

    while (temp && !is_equal(request, temp->request))
    {
        prev = temp;
        temp = temp->next;
    }

    if(!prev)
    {
        PRINT("Error, prev = NULL\n");
        return;
    }

    if(!temp)
    {
        //PRINT("Element  type: %ld, pid_src: %d  is not in list.\n", request->mtype, request->pid_src);
        return;
    }

    list_pointer del = temp;
    prev->next = temp->next;
    list_destroy(del);
}

list_pointer list_find(list_pointer first, long mtype, int pid)
{
    if(!first)
    {
        PRINT("Error, first = NULL\n");
        return NULL;
    } 

    list_pointer temp = first->next;

    while (temp && ((temp->request->mtype != mtype) || (temp->request->pid_src != pid)))
    {
        temp = temp->next;
    }

    if(!temp)
    {
        //PRINT("Element  type: %ld, pid_src: %d  is not in list.\n", mtype, pid);
        return NULL;
    }
    return temp;
}

void print_list(list_pointer first)
{
    while(first && first->next)
    {
        print_msg(first->next->request);
        first = first->next;
    }
}


bool is_equal(msg_pointer request1, msg_pointer request2)
{
    if(request1->mtype != request2->mtype)
        return false;
    if(request1->pid_src != request2->pid_src)
        return false;
    /*if(request1->timestamp != request2->timestamp)
        return false;
    if(strcmp(request1->mtext, request2->mtext) != 0)
        return false;*/

    return true;
}

void update_timestamp(struct process_data* my_data_pointer, int new_timestamp)
{
    if(my_data_pointer->timestamp < new_timestamp)
        my_data_pointer->timestamp = new_timestamp + 1;
    else
        my_data_pointer->timestamp += 1;
}

void send_reply_msgs(struct process_data* my_data_pointer)
{
    list_pointer temp_ptr = list_find(my_data_pointer->list_of_current_requests, REQ, my_data_pointer->msgq_right_neighbour);
    if(temp_ptr != NULL)
    {
        msg_pointer buffer = NULL;
        buffer = temp_ptr->request;
        print_tabs(my_data_pointer->my_no);
        PRINT("%d sending REP message to %d my timestamp: %d\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, my_data_pointer->msgq_right_neighbour), my_data_pointer->timestamp);
        msg_pointer msg = create_msg(REP, my_data_pointer->my_pid, my_data_pointer->timestamp, switch_fork(buffer->mtext[0]));
        char buff[MAX_MESSAGE_SIZE];
        strcpy(buff, struct_to_char(msg));
        write(my_data_pointer->right_fd[1], &buff, sizeof(buff));

        list_remove(my_data_pointer->list_of_current_requests, buffer);
        free(msg);
    }

    temp_ptr = list_find(my_data_pointer->list_of_current_requests, REQ, my_data_pointer->msgq_left_neighbour);
    if(temp_ptr != NULL)
    {
        msg_pointer buffer = NULL;
        buffer = temp_ptr->request;
        print_tabs(my_data_pointer->my_no);
        PRINT("%d sending REP message to %d my timestamp: %d\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, my_data_pointer->msgq_left_neighbour), my_data_pointer->timestamp);
        msg_pointer msg = create_msg(REP, my_data_pointer->my_pid, my_data_pointer->timestamp, switch_fork(buffer->mtext[0]));
        char buff[MAX_MESSAGE_SIZE];
        strcpy(buff, struct_to_char(msg));
        write(my_data_pointer->left_fd[1], &buff, sizeof(buff));

        list_remove(my_data_pointer->list_of_current_requests, buffer);
        free(msg);
    }

    list_destroy_all(my_data_pointer->list_of_current_requests);

    /*msg_pointer msg = create_msg(REP, my_data_pointer->msgq_right_neighbour, 0, "");
    list_remove(my_data_pointer->list_of_current_requests, msg);
    free(msg);
    msg = create_msg(REP, my_data_pointer->msgq_left_neighbour, 0, "");
    list_remove(my_data_pointer->list_of_current_requests, msg);
    free(msg);

    msg_pointer q = create_msg(REQ, my_data_pointer->my_pid, 0, "");
    list_remove(my_data_pointer->list_of_current_requests, q);
    list_remove(my_data_pointer->list_of_current_requests, q);
    free(q);*/
}

bool check_if_first(struct process_data* my_data_pointer)
{
    list_pointer first = my_data_pointer->list_of_current_requests->next;

    int left_flag = 0;
    int right_flag = 0;
    while(first)
    {
        if(first->request->pid_src == my_data_pointer->my_pid)
        {
            if(first->request->mtext == NULL)
            {
                PRINT("Error, first->request->mtext is NULL\n");
                return false;
            } 
            if(strcmp(first->request->mtext, "R") == 0)
                right_flag = 1;
            else if(strcmp(first->request->mtext, "L") == 0)
                left_flag = 1;
        }
        else 
        {
            if(first->request->pid_src == my_data_pointer->msgq_right_neighbour)
            {
                /*if(right_flag != 1)
                    return false;*/
                if(first->request->mtype == REP)        // if REQ nothing happens, our req was before, but we didnt receive REP yet
                    my_data_pointer->forks[1] = 1; 
            }
            else if(first->request->pid_src == my_data_pointer->msgq_left_neighbour)
            {
                /*if(left_flag != 1)
                    return false;*/
                if(first->request->mtype == REP)        // if REQ nothing happens, our req was before, but we didnt receive REP yet
                    my_data_pointer->forks[0] = 1; 
            }
        }
        if(my_data_pointer->forks[0] && my_data_pointer->forks[1])
            return true;

        first = first->next;
    }

    return false;
}

void print_msg(msg_pointer q)
{
    PRINT("%ld, ", q->mtype);
    PRINT("%d, ", q->pid_src);
    PRINT("%d, ", q->timestamp);
    PRINT("%s\n", q->mtext);
}

void print_tabs(int n)
{
    for(int i = 0; i < n-1; i++)
    {
        printf("\t");
    }
    //fflush(stdout);
}

int no_from_pid(int* list_of_pids, int pid)
{
    for(int i = 0; i < 2 * N; i += 2)
    {
        if(list_of_pids[i] == pid)
            return list_of_pids[i + 1]; 
    }

    PRINT("Error, no such pid: ");
    PRINT("%d\n", pid);
    return -1;
}

char* typeno_to_char(long type)
{
    char* temp = (char*)malloc(8 * sizeof(char));
    if(!temp)
    {
        PRINT("Error, couldn't allocate memory for char* temp\n");
        return NULL;
    } 
    strcpy(temp, "HEHEHEHE");
    if(type == 1)
        strcpy(temp, "REQ");
    else if(type == 2)
        strcpy(temp, "REP");

    return temp;
}

char* struct_to_char(msg_pointer message)
{
    char* buffer = (char*)malloc(MAX_MESSAGE_SIZE * sizeof(char));
    if (!buffer) {
        PRINT("Error, Couldn't allocate char*\n");
        return NULL;
    }

    char* p = buffer;

    char type[MAX_TEXT_SIZE];
    sprintf(type, "%ld", message->mtype);
    strcpy(p, type);
    p += strlen(type);

    strcpy(p, ", ");
    p += 2;

    char pid[MAX_TEXT_SIZE];
    sprintf(pid, "%d", message->pid_src);
    strcpy(p, pid);
    p += strlen(pid);

    strcpy(p, ", ");
    p += 2;

    char timestamp[MAX_TEXT_SIZE];
    sprintf(timestamp, "%d", message->timestamp);
    strcpy(p, timestamp);
    p += strlen(timestamp);

    strcpy(p, ", ");
    p += 2;

    strcpy(p, message->mtext); 

    return buffer;
}

msg_pointer char_to_struct(char* message)
{
    msg_pointer msg = (msg_pointer)malloc(sizeof(struct my_msgbuf));
    if (!msg) {
        PRINT("Error: Couldn't allocate memory for message struct\n");
        return NULL;
    }

    char* token;
    char* rest = message;

    token = strtok_r(rest, ", ", &rest);
    if (token) {
        msg->mtype = atol(token);
    }

    token = strtok_r(rest, ", ", &rest);
    if (token) {
        msg->pid_src = atoi(token); 
    }

    token = strtok_r(rest, ", ", &rest);
    if (token) {
        msg->timestamp = atoi(token); 
    }

    token = strtok_r(rest, ", ", &rest);
    if (token) {
        strncpy(msg->mtext, token, MAX_TEXT_SIZE);
        msg->mtext[MAX_TEXT_SIZE - 1] = '\0'; 
    }

    return msg;
}
