#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//process control block (PCB)
struct pcb 
{
	unsigned int pid;
	char pname[20];
	unsigned int ptimeleft;
	unsigned int ptimearrival;
};

typedef struct pcb pcb;

//queue node
struct dlq_node 
{
	struct dlq_node *pfwd;
	struct dlq_node *pbck;
	struct pcb *data;
}; 

typedef struct dlq_node dlq_node;

//queue
struct dlq 
{
	struct dlq_node *head;
	struct dlq_node *tail;
};

typedef struct dlq dlq;
 
//function to add a pcb to a new queue node
dlq_node * get_new_node (pcb *ndata) 
{
	if (!ndata)
		return NULL;

	dlq_node *new = malloc(sizeof(dlq_node));
	if(!new)
    {
		fprintf(stderr, "Error: allocating memory\n");exit(1);
	}

	new->pfwd = new->pbck = NULL;
	new->data = ndata;
	return new;
}

//function to add a node to the tail of queue
void add_to_tail (dlq *q, dlq_node *new)
{
	if (!new)
		return;

	if (q->head==NULL)
    {
		if(q->tail!=NULL)
        {
			fprintf(stderr, "DLList inconsitent.\n"); exit(1);
		}
		q->head = new;
		q->tail = q->head;
	}
	else 
    {		
		new->pfwd = q->tail;
		new->pbck = NULL;
		new->pfwd->pbck = new;
		q->tail = new;
	}
}

//function to remove a node from the head of queue
dlq_node* remove_from_head(dlq * const q){
	if (q->head==NULL){ //empty
		if(q->tail!=NULL){fprintf(stderr, "DLList inconsitent.\n"); exit(1);}
		return NULL;
	}
	else if (q->head == q->tail) { //one element
		if (q->head->pbck!=NULL || q->tail->pfwd!=NULL) {
			fprintf(stderr, "DLList inconsitent.\n"); exit(1);
		}

		dlq_node *p = q->head;
		q->head = NULL;
		q->tail = NULL;
	
		p->pfwd = p->pbck = NULL;
		return p;
	}
	else { // normal
		dlq_node *p = q->head;
		q->head = q->head->pbck;
		q->head->pfwd = NULL;
	
		p->pfwd = p->pbck = NULL;
		return p;
	}
}

//function to print our queue
void print_q (const dlq *q) 
{
    dlq_node *n = q->head;
    if (n == NULL)
    {
        printf("empty");
        return;
    }        
    while (n) 
    {
        printf("%s(%d),", n->data->pname, n->data->ptimeleft);
        n = n->pbck;
    }
}

//function to check if the queue is empty
int is_empty (const dlq *q)
{
	if (q->head == NULL && q->tail==NULL)
		return 1;
	else if (q->head != NULL && q->tail != NULL)
		return 0;
	else 
    {
		fprintf(stderr, "Error: DLL queue is inconsistent."); exit(1);
	}
}

//function to sort the queue on completion time
void sort_by_timetocompletion(const dlq *q) 
{ 
	// bubble sort
	dlq_node *start = q->tail;
	dlq_node *end = q->head;
	
	while (start != end) 
    {
		dlq_node *node = start;
		dlq_node *pfwd = node->pfwd;

		while (pfwd != NULL) 
        {
			if (node->data->ptimeleft < pfwd->data->ptimeleft)
            {
				// do a swap
				pcb *temp = node->data;
				node->data = pfwd->data;
				pfwd->data = temp;
			}
			node = pfwd;
			pfwd = node->pfwd;
		}
		end = end ->pbck;
	} 
}

//function to sort the queue on arrival time
void sort_by_arrival_time (const dlq *q) 
{
	// bubble sort
	dlq_node *start = q->tail;
	dlq_node *end = q->head;
	
	while (start != end) 
    {
		dlq_node *node = start;
		dlq_node *pfwd = node->pfwd;

		while (pfwd != NULL) 
        {
			if (node->data->ptimearrival < pfwd->data->ptimearrival)
            {
				// do a swap
				pcb *temp = node->data;
				node->data = pfwd->data;
				pfwd->data = temp;
			}
			node = pfwd;
			pfwd = node->pfwd;
		}
		end = end->pbck;
	}
}

//function to tokenize the one row of data
pcb* tokenize_pdata (char *buf) 
{
	pcb* p = (pcb*) malloc(sizeof(pcb));
	if(!p)
    { 
        fprintf(stderr, "Error: allocating memory.\n"); exit(1);
    }

	char *token = strtok(buf, ":\n");
	if(!token)
    { 
        fprintf(stderr, "Error: Expecting token pname\n"); exit(1);
    }  
	strcpy(p->pname, token);

	token = strtok(NULL, ":\n");
	if(!token)
    { 
        fprintf(stderr, "Error: Expecting token pid\n"); exit(1);
    }  
	p->pid = atoi(token);

	token = strtok(NULL, ":\n");
	if(!token)
    { 
        fprintf(stderr, "Error: Expecting token duration\n"); exit(1);
    } 
	 
	p->ptimeleft= atoi(token);

	token = strtok(NULL, ":\n");
	if(!token)
    { 
        fprintf(stderr, "Error: Expecting token arrival time\n"); exit(1);
    }  
	p->ptimearrival = atoi(token);

	token = strtok(NULL, ":\n");
	if(token)
    { 
        fprintf(stderr, "Error: Oh, what've you got at the end of the line?\n");exit(1);
    } 

	return p;
}

void print_q_fifo (const dlq *q , int ptime) 
{
    dlq_node *n = q->head;
    int pcount = 0;
    if (n == NULL)
    {
        printf("empty");
        return;
    }
    while (n) 
    {  
        if (n->data->ptimearrival< ptime) 
        {
            printf("%s(%d),", n->data->pname, n->data->ptimeleft);
            n = n->pbck;
            pcount++;
        }
        else 
        {   
            break;
        }
    }
    if(pcount == 0)
        printf("empty");
}

int count_process(dlq *const q)
{
    dlq_node *n = q->head;
    int result = 0;
    
    while(n)
    {
        result++;
        n = n->pbck;
    }
    return result;
}

void perf_metric(int response_time_q[] , int turnaround_time_q[] , int total_proc , int throughput)
{
    float avg_turnaround;
    float avg_response;
    
    for(int i=0;i<total_proc ; i++)
    {
        avg_turnaround += turnaround_time_q[i];
    
        avg_response+= response_time_q[i];
    }
    
    avg_response = avg_response/total_proc;
    avg_turnaround = avg_turnaround/total_proc;
    
   
    printf("Performance Metrics : \n Throughput: ");
    printf("%d \n Average Response Time: %f ms \n Average Turnaround Time: %f ms \n" , throughput , avg_response, avg_turnaround);
}



//implement the FIFO scheduling code
void sched_FIFO(dlq *const p_fq, int *p_time)
{
    //add code here to implement FIFO scheduling logic
    // printf("FIFO not implemented.");
    
    // This impplementation works completely fine but I did'nt make a ready q for scheduler here
    // if(*p_time == 0)
    // {
    //     (*p_time)++;
    // }
    
    // sort_by_arrival_time(p_fq);
    // dlq_node * current = p_fq->head;
    // remove_from_head(p_fq);
    
    // while ((current->data->ptimearrival >= *p_time))
    // {
    //     printf("%d:idle:empty: \n", *p_time);
    //     (*p_time)++;
    // }
    
    // while (current != NULL)
    // {
    //     pcb *process = current->data;
        
    //     printf("%d:%s:", *p_time, process->pname);
    //     print_q_fifo(p_fq , *p_time);
    //     printf(":\n");
    //     // Update the remaining time for the process
    //     if (process->ptimeleft > 1)
    //     {
    //         process->ptimeleft--;       
    //     }
    //     else
    //     {
    //         current = remove_from_head(p_fq); // Remove the process from the queue
    //         free(process);
    //     }
    //     (*p_time)++;
    // }
    int total_proc = count_process(p_fq);
    int response_time_q [total_proc];
    int turnaround_time_q[total_proc];
    int limit = 50 ; // setting limit to 50 millisecond for calculating throughput
    int throughput;
    //Initializing these arrays with arival time;
    
    dlq* myq = (dlq*)malloc(sizeof(p_fq));
    myq->head = get_new_node(NULL);
    myq->tail = myq->head;
    
    sort_by_arrival_time(p_fq);
    dlq_node *current = remove_from_head(p_fq);
    
    
    response_time_q[0] = turnaround_time_q[0] = current->data->ptimearrival;
    int arrival = 0;
    int now = 0;
    char check = '1';
    
    while (current != NULL)
    {
        (*p_time)++; 

        //move to myq (ready queue) when the arrival time == system time
        while (!is_empty(p_fq) && p_fq->head->data->ptimearrival < *p_time) 
        {
            dlq_node *temp = remove_from_head(p_fq);
            add_to_tail(myq, temp);
            arrival +=1;
            response_time_q[arrival] = turnaround_time_q[arrival] = temp->data->ptimearrival;
            
        }

        // If no process has not arrived, print idle and continue
        if (current->data->ptimearrival >= *p_time) 
        {
            printf("%d:idle:empty:\n", *p_time);
            continue;
        }

        if(check == '1')
        {
            response_time_q[now] = *p_time - response_time_q[now]-1;
            check = '0';
        }
       
        pcb *process = current->data;
        // Print the state of the selected process
        printf("%d:%s:", *p_time, process->pname);
        print_q(myq);
        printf(":\n");

        // Update the remaining time for the process
        if (process->ptimeleft > 1) 
        {
            process->ptimeleft--;
        } 
        else 
        {
            turnaround_time_q[now] = *p_time - turnaround_time_q[now];
            now++;
            check = '1';
            
            if (*p_time < limit)
            {
                throughput = now;
            }
            
            free(process);
            current = remove_from_head(myq);
        }
  
    }
    
    perf_metric(response_time_q , turnaround_time_q , total_proc , throughput );
    
    
}
//implement the SJF scheduling code
void sched_SJF(dlq *const p_fq, int *p_time)
{
    //add code here to implement SJF scheduling logic
    // printf("SJF not implemented.");
 
    int total_proc = count_process(p_fq);
    int response_time_q [total_proc];
    int turnaround_time_q[total_proc];
    int limit = 50 ; // setting limit to 50 millisecond for calculating throughput
    int throughput;

    // Create myq for schedler
    
    dlq* myq = (dlq*)malloc(sizeof(p_fq));
    myq->head = get_new_node(NULL);
    myq->tail = myq->head;
    
    sort_by_arrival_time(p_fq);
    dlq_node *current = remove_from_head(p_fq);
    
    response_time_q[0] = turnaround_time_q[0] = current->data->ptimearrival;
    int arrival = 0;
    int now = 0;
    char check = '1';
    
    while (current != NULL) {
        (*p_time)++; 

        //move to myq (ready queue) when the arrival time == system time
        while (!is_empty(p_fq) && p_fq->head->data->ptimearrival < *p_time) {
            dlq_node *temp = remove_from_head(p_fq);
            add_to_tail(myq, temp);
            
            arrival +=1;
            response_time_q[arrival] = turnaround_time_q[arrival] = temp->data->ptimearrival;
        }

        // If no process has not arrived, print idle and continue
        if (current->data->ptimearrival >= *p_time) {
            printf("%d:idle:empty:\n", *p_time);
            continue;
        }
        
        if(check == '1')
        {
            response_time_q[now] = *p_time - response_time_q[now]-1;
            check = '0';
        }

        // Sort myq by remaining time (SJF)
        sort_by_timetocompletion(myq);

        // Get the process with the shortest remaining time
        
        pcb *process = current->data;

        // Print the state of the selected process
        printf("%d:%s:", *p_time, process->pname);
        print_q(myq);
        printf(":\n");

        // Update the remaining time for the process
        if (process->ptimeleft > 1) 
        {
            process->ptimeleft--;
        } 
        
        else 
        {
            turnaround_time_q[now] = *p_time - turnaround_time_q[now];
            now++;
            check = '1';
            
            if (*p_time < limit)
            {
                throughput = now;
            }
            
            free(process);
            current = remove_from_head(myq);
        }

        
    }
    
    perf_metric(response_time_q , turnaround_time_q , total_proc , throughput );
}
//implement the STCF scheduling code
void sched_STCF(dlq *const p_fq, int *p_time)
{
    //add code here to implement STCF scheduling logic
    // printf("STCF not implemented.");
    
    int total_proc = count_process(p_fq);
    int response_time_q [total_proc];
    int turnaround_time_q[total_proc];
    int limit = 50 ; // setting limit to 50 millisecond for calculating throughput
    int throughput;
    
    
    dlq* myq = (dlq*)malloc(sizeof(p_fq));
    myq->head = get_new_node(NULL);
    myq->tail = myq->head;
    
    // sort_by_arrival_time(p_fq);
    dlq_node *current = remove_from_head(p_fq);
    
    response_time_q[0] = turnaround_time_q[0] = current->data->ptimearrival;
    int arrival = 0;
    int now = 0;
    char check[total_proc];
    int int_check = 0;
    
    check[0] = current->data->pid; 
    
    while (current != NULL) {
        (*p_time)++; 

        //move to myq (ready queue) when the arrival time == system time
        while (!is_empty(p_fq) && p_fq->head->data->ptimearrival == *p_time-1) {
            
            dlq_node *temp = remove_from_head(p_fq);
            // printf("%s \n",temp->data->pname);
            arrival +=1;
            check[arrival] = temp->data->pid;
            response_time_q[arrival] = turnaround_time_q[arrival] = temp->data->ptimearrival;
            
            add_to_tail(myq, temp);
            sort_by_timetocompletion(myq);
            
            
            int var1 = (current->data->ptimeleft);
            if(  (var1 > (myq->head->data->ptimeleft)))
            {
                add_to_tail(myq, current);
                current = remove_from_head(myq);
                
                sort_by_timetocompletion(myq);
                
            }
        }

        // If no process has not arrived, print idle and continue
        if (current->data->ptimearrival >= *p_time) {
            printf("%d:idle:empty:\n", *p_time);
            continue;
        }
        
        if((check[int_check]==current->data->pid) & (int_check < total_proc))
            {
                response_time_q[int_check] = *p_time - response_time_q[int_check]-1;
                // printf("%s,%d \n",current->data->pname,response_time_q[int_check]);
                int_check++;
            }

        // Get the process with the shortest remaining time
      
        pcb *process = current->data;

        // Print the state of the selected process
        printf("%d:%s:", *p_time, process->pname);
        print_q(myq);
        printf(":\n");

        // Update the remaining time for the process
        if (process->ptimeleft > 1) 
            process->ptimeleft--;
        else 
        {
               
            turnaround_time_q[now] = *p_time - turnaround_time_q[now];
            now++;
            
            if (*p_time < limit)
            {
                throughput = now;
            }
            
            free(process);
            current = remove_from_head(myq);
        }

        
    }

    
    perf_metric(response_time_q , turnaround_time_q , total_proc , throughput );
}

//implement the RR scheduling code
void sched_RR(dlq *const p_fq, int *p_time)
{
    //add code here to implement RR scheduling logic
    // printf("RR not implemented.");
    
    int total_proc = count_process(p_fq);
    int response_time_q [total_proc];
    int turnaround_time_q[total_proc];
    int limit = 50 ; // setting limit to 50 millisecond for calculating throughput
    int throughput;
    
    
    dlq* myq = (dlq*)malloc(sizeof(p_fq));
    myq->head = get_new_node(NULL);
    myq->tail = myq->head;
    
    sort_by_arrival_time(p_fq);
    dlq_node *current = remove_from_head(p_fq);
    
    response_time_q[0] = turnaround_time_q[0] = current->data->ptimearrival;
    int arrival = 0;
    int now = 0;
    int check[total_proc];
    int int_check = 0;
    
    check[0] = current->data->pid; 
    
    
    while (current != NULL)
    {
        (*p_time)++; 

        //move to myq (ready queue) when the arrival time == system time
        while (!is_empty(p_fq) && p_fq->head->data->ptimearrival == *p_time - 1) 
        {
            dlq_node *temp = remove_from_head(p_fq);
            add_to_tail(myq, temp);
            // printf("%d \n",temp->data->pid);
            
            arrival +=1;
            check[arrival] = temp->data->pid;
            response_time_q[arrival] = turnaround_time_q[arrival] = temp->data->ptimearrival;
        }

        // If no process has not arrived, print idle and continue
        if (current->data->ptimearrival >= *p_time) 
        {
            printf("%d:idle:empty:\n", *p_time);
            continue;
        }
        
        if((check[int_check]==current->data->pid) & (int_check < total_proc))
            {
                response_time_q[int_check] = *p_time - response_time_q[int_check]-1;
                // printf("%s,%d \n",current->data->pname,response_time_q[int_check]);
                int_check++;
            }
        
        pcb *process = current->data;

        // Print the state of the selected process
        printf("%d:%s:", *p_time, process->pname);
        print_q(myq);
        printf(":\n");
        
        //update the remaining time
        process->ptimeleft--;

        if (process->ptimeleft == 0) 
        {
            turnaround_time_q[now] = *p_time - turnaround_time_q[now];
            now++;
            
            if (*p_time < limit)
            {
                throughput = now;
            }
            
            current = remove_from_head(myq);
            
        } 
        else 
        {    
            current->data->ptimearrival = *p_time;
            add_to_tail(myq, current);
            current = remove_from_head(myq);
        }

    }
    
    perf_metric(response_time_q , turnaround_time_q , total_proc , throughput );
}


int main()
{
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    int N = 0;
    char tech[20]={'\0'};
    char buffer[100]={'\0'};
    scanf("%d", &N);
    scanf("%s", tech);
     
    dlq queue;
    queue.head = NULL;
    queue.tail = NULL;
    for(int i=0; i<N; ++i)
    {   
        scanf("%s\n", buffer); 
        // printf("%s\n",buffer);
        pcb *p = tokenize_pdata(buffer);
        add_to_tail (&queue, get_new_node(p) );  
    }
    //print_q(&queue);
    unsigned int system_time = 0;
    sort_by_arrival_time (&queue);
    //print_q (&queue);
    
    // run scheduler
    if(!strncmp(tech,"FIFO",4))
        sched_FIFO(&queue, &system_time);
    else if(!strncmp(tech,"SJF",3))
        sched_SJF(&queue, &system_time);
    else if(!strncmp(tech,"STCF",4))
        sched_STCF(&queue, &system_time);
    else if(!strncmp(tech,"RR",2))
        sched_RR(&queue, &system_time);
    else
        fprintf(stderr, "Error: unknown POLICY\n");
    return 0;
}