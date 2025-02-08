#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define N 5
#define MAX_MESSAGE_SIZE (sizeof(struct my_msgbuf) - sizeof(long))
#define MAX_TEXT_SIZE 10

//mtype
#define REQ 1
#define REP 2
#define EXIT 3

struct my_msgbuf {
	long mtype;       /* message type, must be > 0 */
    int pid_src;
    int msgqid_src;
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
	int msgqid;
	int* list_of_pids;
	list_pointer list_of_current_requests;
    int my_no;

	int my_pid;

	int msgq_right_neighbour[2];
	int msgq_left_neighbour[2];
};

void* create_children(void*);
void* create_shared_memory(size_t size);
void prepare(int* list_of_pids, struct process_data* my_data, int* sync);
void print_list_of_pids(struct process_data* my_data);
void think(struct process_data* my_data_pointer);
void get_forks(struct process_data* my_data_pointer);
void eat(struct process_data* my_data_pointer);
int next_msgqid(int* list_of_pids, int curr); // Right
int prev_msgqid(int* list_of_pids, int curr); // Left
int next_pid(int* list_of_pids, int curr);    // R
int prev_pid(int* list_of_pids, int curr);    // L
int no_from_pid(int* list_of_pids, int pid);
int receive_msg(struct process_data* my_data_pointer);
bool is_equal(msg_pointer request1, msg_pointer request2);
void update_timestamp(struct process_data* my_data_pointer, int new_timestamp);
void process_msg(struct process_data* my_data_pointer, msg_pointer buffer);
void send_exit_msgs(struct process_data* my_data_pointer);
bool check_if_first(struct process_data* my_data_pointer);

msg_pointer create_msg(long mtype, int pid_src, int msgqid_src, int timestamp, char mtext[MAX_TEXT_SIZE]);
void delete_msg(msg_pointer msg);

list_pointer list_create(msg_pointer request);
void list_add(list_pointer list, msg_pointer request);
void list_remove(list_pointer list, msg_pointer request);
void list_destroy(list_pointer list_element);
void list_destroy_all(list_pointer head);
void print_list(list_pointer first);
void print_msg(msg_pointer q);
void print_tabs(int n);

char* typeno_to_char(long type);

int main()
{
    //msg_pointer request = create_msg(-1, -1, -1, -1, "");
    //struct list_element Head = {.request = request, .next = NULL};
    struct list_element Head = {.request = NULL, .next = NULL};
    list_pointer first = &Head;

	struct process_data my_data = {.timestamp = 1, .forks = {0, 0}, .my_no = -1, .msgqid = 0, .list_of_pids = NULL, .list_of_current_requests = first, .my_pid = 0, .msgq_right_neighbour = { 0 }, .msgq_left_neighbour = { 0 }};
	struct process_data* my_data_pointer = &my_data;
	void* shared_mem = NULL;
    void* shared_sync = NULL;

	shared_mem = create_shared_memory(3*N*sizeof(int));			// [{pid1, msgq1}, {pid2, msgq2}, ...]
    shared_sync = create_shared_memory(sizeof(int));
    memset(shared_sync, 0, sizeof(int));

	shared_mem = create_children(shared_mem);

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

void* create_children(void* shared_mem)
{
	int* list_of_pids = NULL;
	pid_t process_pid = 0;
	pid_t pid = 0;

	list_of_pids = (int*)(calloc(3 * N, sizeof(int)));
	if(list_of_pids == NULL)
	{
		printf("ERROR Can't allocate memory for list_of_pids!\n");
		return NULL;
	}
	
	list_of_pids[0] = getpid();
	for(int i = 1; i < N; i++)
	{
		pid = fork();
		if(pid < 0)
			return NULL;
		else if(pid == 0)
		{	
			break;
		}
		else
		{
			list_of_pids[3 * i] = pid;
		}
	}
	if(pid != 0)
		memcpy(shared_mem, list_of_pids, 3*N*sizeof(int));

    free(list_of_pids);
	return shared_mem;
}

void prepare(int* list_of_pids, struct process_data* my_data, int* sync)
{	
	int msgqid = msgget(getpid(), IPC_CREAT|00622); 	//others can write, the owner can read and write
	my_data->forks[0] = 0;
	my_data->forks[1] = 0;
	my_data->msgqid = msgqid;
	my_data->list_of_pids = list_of_pids;
	my_data->my_pid = getpid();
	my_data->msgq_right_neighbour[0] = 0;
	my_data->msgq_right_neighbour[1] = 0;
 	my_data->msgq_left_neighbour[0] = 0;
	my_data->msgq_left_neighbour[1] = 0;
	for(int i = 0; i < N; i++)
	{
		while(my_data->list_of_pids[3*i] == 0);			// to ensure that the parent process has put all pids in the list
	}

	for(int i = 0; i < N; i++)
	{
		if(my_data->list_of_pids[3 * i] == my_data->my_pid)
		{
            my_data->my_no = i+1;
            my_data->timestamp = my_data->my_no;
            my_data->list_of_pids[3*i+2] = my_data->my_no;
			my_data->list_of_pids[3*i+1] = msgqid;
			for(int j = 0; j < N; j++)
			{
				while(my_data->list_of_pids[3*j + 1] == 0);  // wait for everyone to do it
			}
		}
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

		if(my_data->my_pid == my_data->list_of_pids[3*i])
		{
			my_data->msgq_right_neighbour[0] = next_pid(my_data->list_of_pids, 3*i);
 			my_data->msgq_left_neighbour[0] = prev_pid(my_data->list_of_pids, 3*i);
			my_data->msgq_right_neighbour[1] = next_msgqid(my_data->list_of_pids, 3*i);
 			my_data->msgq_left_neighbour[1] = prev_msgqid(my_data->list_of_pids, 3*i);
		}
	}

	return;
}

void print_list_of_pids(struct process_data* my_data)
{
	printf("pids: \n");
	for(int i = 0; i < N; i++)
	{
		printf("%d ", my_data->list_of_pids[3 * i]);
	}

	printf("\nmsgqids: \n");
	for(int i = 0; i < N; i++)
	{
		printf("%d ", my_data->list_of_pids[3 * i + 1]);
	}

    printf("\nno: \n");
	for(int i = 0; i < N; i++)
	{
		printf("%d ", my_data->list_of_pids[3 * i + 2]);
	}
	
	printf("\n\n\n\n");	
}

void think(struct process_data* my_data_pointer)
{	
	int upper_bound = 5, lower_bound = 2;
	srand(my_data_pointer->my_pid);
	int time_to_think = rand() % (upper_bound - lower_bound + 1) + lower_bound;
    //time_to_think=0;
    print_tabs(my_data_pointer->my_no);
	printf("%d thinking for %ds\n", my_data_pointer->my_no, time_to_think);

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
	//printf("%d trying to get forks(%d, %d)\n", my_data_pointer->my_no, my_data_pointer->forks[0], my_data_pointer->forks[1]);

    receive_msg(my_data_pointer);

    if(my_data_pointer->forks[0] == 0)
    {	
        print_tabs(my_data_pointer->my_no);
        printf("%d sending REQ message to %d (my timestamp: %d)\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, my_data_pointer->msgq_left_neighbour[0]), my_data_pointer->timestamp);
        msg_pointer msg = create_msg(REQ, my_data_pointer->my_pid, my_data_pointer->msgqid, my_data_pointer->timestamp, "L");
        msgsnd(my_data_pointer->msgq_left_neighbour[1], msg, MAX_MESSAGE_SIZE, 0);
        list_add(my_data_pointer->list_of_current_requests, msg);
    }
    if(my_data_pointer->forks[1] == 0)
    {	
        print_tabs(my_data_pointer->my_no);
        printf("%d sending REQ message to right %d (my timestamp: %d)\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, my_data_pointer->msgq_right_neighbour[0]), my_data_pointer->timestamp);
        msg_pointer msg = create_msg(REQ, my_data_pointer->my_pid, my_data_pointer->msgqid, my_data_pointer->timestamp, "R");
        msgsnd(my_data_pointer->msgq_right_neighbour[1], msg, MAX_MESSAGE_SIZE, 0);
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

    print_tabs(my_data_pointer->my_no);
	printf("%d eating for %ds\n", my_data_pointer->my_no, time_to_eat);

    /*printf("LIST: \n");
    print_list(my_data_pointer->list_of_current_requests);
    printf("flags: %d %d\n", my_data_pointer->left_flag, my_data_pointer->right_flag);
    printf("\n\n\n");*/
    //time_to_eat=0;
    sleep(time_to_eat);

    my_data_pointer->forks[0] = 0;
    my_data_pointer->forks[1] = 0;
    send_exit_msgs(my_data_pointer);

	return think(my_data_pointer);
}

int next_msgqid(int* list_of_pids, int curr)
{
	if((curr + 4) >= 3*N)
	{
		return list_of_pids[1];
	}
	return list_of_pids[(curr + 4)];
}
int prev_msgqid(int* list_of_pids, int curr)
{
	if((curr - 2) < 0)
	{
		return list_of_pids[3*N-2];
	}
	return list_of_pids[(curr - 2)];
}

int next_pid(int* list_of_pids, int curr)
{
	if((curr + 3) >= 3*N)
	{
		return list_of_pids[0];
	}
	return list_of_pids[(curr + 3)];
}
int prev_pid(int* list_of_pids, int curr)
{
	if((curr - 3) < 0)
	{
		return list_of_pids[3*N-3];
	}
	return list_of_pids[(curr - 3)];
}

int receive_msg(struct process_data* my_data_pointer)
{	
	msg_pointer buffer = create_msg(0, 0, 0, 0, "");
	ssize_t size_of_msg = 0;

	size_of_msg = msgrcv(my_data_pointer->msgqid, buffer, MAX_MESSAGE_SIZE, 0, IPC_NOWAIT);
	if(size_of_msg != -1)
	{
        print_tabs(my_data_pointer->my_no);
		printf("%d receives a %s message from %d, my timestamp: %d, his timestamp: %d\n", my_data_pointer->my_no, typeno_to_char(buffer->mtype), no_from_pid(my_data_pointer->list_of_pids, buffer->pid_src), my_data_pointer->timestamp, buffer->timestamp);

        process_msg(my_data_pointer, buffer);
	}

    return 0;
}

void process_msg(struct process_data* my_data_pointer, msg_pointer buffer)
{
    update_timestamp(my_data_pointer, buffer->timestamp);
    if(buffer->mtype == REQ)
    {
        list_add(my_data_pointer->list_of_current_requests, buffer);
        print_tabs(my_data_pointer->my_no);
        printf("%d sending REP message to %d my timestamp: %d\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, buffer->pid_src), my_data_pointer->timestamp);
		msg_pointer msg = create_msg(REP, my_data_pointer->my_pid, my_data_pointer->msgqid, my_data_pointer->timestamp, "REP");
		msgsnd(buffer->msgqid_src, msg, MAX_MESSAGE_SIZE, 0);
        free(msg);
    }
    else if(buffer->mtype == REP)
    {
		list_add(my_data_pointer->list_of_current_requests, buffer);
    }
    else if(buffer->mtype == EXIT)
    {
        buffer->mtype = REQ;
		list_remove(my_data_pointer->list_of_current_requests, buffer);
    }
    free(buffer);
}

msg_pointer create_msg(long mtype, int pid_src, int msgqid_src, int timestamp, char mtext[MAX_TEXT_SIZE])
{
    msg_pointer q = (msg_pointer)malloc(sizeof(struct my_msgbuf));
    if(!q)
    {
        printf("Error, Couldn't allocate msg!\n");
        return NULL;
    }

    q->mtype = mtype;
    q->pid_src = pid_src;
    q->msgqid_src = msgqid_src;
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
        printf("Error, Couldn't allocate memory for list_element!\n");
        return NULL;
    }

    q->request = create_msg(request->mtype, request->pid_src, request->msgqid_src, request->timestamp, request->mtext);
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
        printf("Error, first = NULL\n");
        return;
    }

    list_pointer new = list_create(request);
    if(!new)
    {
        printf("Fail in list_add, new = NULL\n");
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
        printf("Error, first = NULL\n");
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
        printf("Error, prev = NULL\n");
        return;
    }

    if(!temp)
    {
        //printf("Element  type: %ld, pid_src: %d  is not in list.\n", request->mtype, request->pid_src);
        return;
    }

    list_pointer del = temp;
    prev->next = temp->next;
    /*printf("List remove: ");
    print_msg(del->request);
    printf("\n");*/
    list_destroy(del);
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
    if(request1->msgqid_src != request2->msgqid_src)
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

void send_exit_msgs(struct process_data* my_data_pointer)
{
    msg_pointer msg = create_msg(REP, my_data_pointer->msgq_right_neighbour[0], my_data_pointer->msgq_right_neighbour[1], 0, "");
    print_tabs(my_data_pointer->my_no);
    printf("%d sending EXIT message to right %d my timestamp: %d\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, my_data_pointer->msgq_right_neighbour[0]), my_data_pointer->timestamp);
    msg_pointer msg_send = create_msg(EXIT, my_data_pointer->my_pid, my_data_pointer->msgqid, my_data_pointer->timestamp, "EXIT");
    msgsnd(my_data_pointer->msgq_right_neighbour[1], msg_send, MAX_MESSAGE_SIZE, 0);
    list_remove(my_data_pointer->list_of_current_requests, msg);
    free(msg);
    free(msg_send);

    msg_pointer msg_2 = create_msg(REP, my_data_pointer->msgq_left_neighbour[0], my_data_pointer->msgq_left_neighbour[1], 0, "");
    print_tabs(my_data_pointer->my_no);
    printf("%d sending EXIT message to left %d my timestamp: %d\n", my_data_pointer->my_no, no_from_pid(my_data_pointer->list_of_pids, my_data_pointer->msgq_left_neighbour[0]), my_data_pointer->timestamp);
    msg_send = create_msg(EXIT, my_data_pointer->my_pid, my_data_pointer->msgqid, my_data_pointer->timestamp, "EXIT");
    msgsnd(my_data_pointer->msgq_left_neighbour[1], msg_send, MAX_MESSAGE_SIZE, 0);
    list_remove(my_data_pointer->list_of_current_requests, msg_2);
    free(msg_2);
    free(msg_send);

    msg_pointer my_1 = create_msg(REQ, my_data_pointer->my_pid, my_data_pointer->msgqid, 0, "");
    list_remove(my_data_pointer->list_of_current_requests, my_1);
    list_remove(my_data_pointer->list_of_current_requests, my_1);
    free(my_1);
}

bool check_if_first(struct process_data* my_data_pointer)
{
    list_pointer first = my_data_pointer->list_of_current_requests->next;
    
    int left_flag = 0;
    int right_flag = 0;

    while(first)
    {
        //printf("pid: %d, first: ", my_data_pointer->my_pid);
        //print_element(first);
        if(first->request->pid_src == my_data_pointer->my_pid)
        {
            if(first->request->mtext == NULL)
            {
                printf("Error, first->request->mtext is NULL\n");
                return false;
            } 
            if(strcmp(first->request->mtext, "R") == 0)
                right_flag = 1;
            else if(strcmp(first->request->mtext, "L") == 0)
                left_flag = 1;
        }
        else 
        {
            if(first->request->msgqid_src == my_data_pointer->msgq_right_neighbour[1])
            {
                if(right_flag != 1)
                    return false;
                if(first->request->mtype == REP)        // if REQ nothing happens, our req was before, but we didnt receive REP yet
                    my_data_pointer->forks[1] = 1; 
            }
            else if(first->request->msgqid_src == my_data_pointer->msgq_left_neighbour[1])
            {
                if(left_flag != 1)
                    return false;
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
    printf("%ld, ", q->mtype);
    printf("%d, ", q->pid_src);
    printf("%d, ", q->msgqid_src);
    printf("%d, ", q->timestamp);
    printf("%s\n", q->mtext);
}

void print_tabs(int n)
{
    for(int i = 0; i < n-1; i++)
    {
        printf("\t");
    }
}

int no_from_pid(int* list_of_pids, int pid)
{
    for(int i = 0; i < 3 * N; i += 3)
    {
        if(list_of_pids[i] == pid)
            return list_of_pids[i + 2]; 
    }

    printf("Error, no such pid\n");
    return -1;
}

char* typeno_to_char(long type)
{
    char* temp = (char*)malloc(8 * sizeof(char));
    if(!temp)
    {
        printf("Error, couldn't allocate memory for char* temp\n");
        return NULL;
    } 
    if(type == 1)
        strcpy(temp, "REQ");
    else if(type == 2)
        strcpy(temp, "REP");
    else
        strcpy(temp, "EXIT");

    return temp;
}
