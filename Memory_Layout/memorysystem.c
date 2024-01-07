#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MEMSIZE 500
#define MAXFRAMES 5

/** Assumptions **/

/* 1. The stack and heap memory is represented by a character array of size 500. 
   2. The initial 10 bytes of stack frames are reserved for system values like function return address.


/***Structs****/

struct framestatus {
	int number;               // frame number
       	char name[8];             // function name representing the frame
	int functionaddress;      // address of function in code section (will be randomly generated in this case)
	int frameaddress;         // starting address of frame belonging to this header in Stack
	bool used;                 // a boolean value indicating wheter the frame status entry is in use or not
};

struct freelist {
	int start;         // start address of free region
	int size;                 // size of free region
	struct freelist * next;   // pointer to next free region
};

struct allocated{
 	char name[8];
	int startaddress;
	struct allocated * next;

};

struct mem_stack{
	struct framestatus frames[MAXFRAMES];
	// struct stackframe stackframes[MAXFRAMES];
	int top; //top / current location of stack of stack
};


/****Helper functions for free list ******/


// Function to initialize a new node
struct freelist* create_node(int start, int size) {
    struct freelist *newNode = (struct freelist*)malloc(sizeof(struct freelist));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    newNode->start = start;
    newNode->size = size;
    newNode->next = NULL;

    return newNode;
}

// Function to insert a node at the end of the list
struct freelist* insert_node(struct freelist *head, int start, int size) {
    struct freelist *newNode = create_node(start, size);

    // If the list is empty, make the new node the head
    if (head == NULL) {
        return newNode;
    }
	// printf("I am in insertnodefree/n");
    // Traverse to the end of the list
    struct freelist *current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = newNode;
    return head;
}

// Function to iterate through the list and print each node
void print_free(struct freelist *head) {
    struct freelist *current = head;
    while (current != NULL) {
        printf("(Start: %d, Size: %d) , ", current->start, current->size);
        current = current->next;
    }

	printf("\n");
}

// Function to free the memory occupied by the list
void free_freelist(struct freelist *head) {
    struct freelist *current = head;
    struct freelist *nextNode;

    while (current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
}

// Function to merge adjacent nodes  addresses to have one big free space
struct freelist* merge_nodes(struct freelist *head) {
    struct freelist *current = head;
	// printf("I am in merge /n");
    while (current != NULL && current->next != NULL) {
        struct freelist *nextNode = current->next;

        // Check if the current node and the next node overlap
        if (current->start + current->size == nextNode->start) {
            // Merge the nodes
            current->size = nextNode->start + nextNode->size - current->start;
            current->next = nextNode->next;
            free(nextNode);
        } else {
            current = current->next;
        }
    }

    return head;
}

struct freelist* find_space(struct freelist *head, int required_size) {
    struct freelist *current = head;

    while (current != NULL) {
        if (current->size >= required_size) {
            return current;
        }

        current = current->next;
    }

    return NULL; // No node with enough free space found
}


/****Helper functions for allocated list ******/



// Function to initialize a new node
struct allocated *create_allocated_node(const char *name, int startaddress) {
    struct allocated *newNode = (struct allocated *)malloc(sizeof(struct allocated));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    strcpy(newNode->name, name);
    newNode->startaddress = startaddress;
    newNode->next = NULL;

    return newNode;
}

// Function to insert a node at the end of the list
struct allocated *insert_allocated_node(struct allocated *head, const char *name, int startaddress) {
    struct allocated *newNode = create_allocated_node(name, startaddress);

    // If the list is empty, make the new node the head
    if (head == NULL ) {
        return newNode;
    }

    // Traverse to the end of the list
    struct allocated *current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = newNode;

    return head;
}



// Function to delete a node by name
struct allocated *delete_allocated(struct allocated *head, const char *name) {
    struct allocated *current = head;
    struct allocated *prev = NULL;
	// printf("I am in dealocate /n");
    // Traverse the list to find the node with the given name
    while (current != NULL && strcmp(current->name, name) != 0) {
        prev = current;
        current = current->next;
    }

    // If the node is found, remove it from the list
    if (current != NULL) {
        if (prev != NULL) {
            prev->next = current->next;
        } else {
            // If the node to be deleted is the head, update the head
            head = current->next;
        }

        free(current);
    }

    return head;
}

// Function to print the list
void print_allocated(const struct allocated *head) {
    const struct allocated *current = head->next;
    while (current != NULL) {
        printf("(Name: %s, Start Address: %d), ", current->name, current->startaddress);
        current = current->next;
    }
	printf("\n");
}

// Function to free the memory occupied by the list
void free_allocated(struct allocated *head) {
    struct allocated *current = head;
    struct allocated *nextNode;

    while (current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
}

// Function to find the node by name
struct allocated* find_allocated_node(struct allocated *head, const char *name) {
    struct allocated *current = head;
	// printf("head name: %s\n", head->name);
	

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
			// printf("current name: %s\n", current->name);
            return current;
        }

        current = current->next;
    }

    return NULL; // No node with the given name found
}



/***Global variables****/

int currentstacksize = 105;
int currentheapsize = 100;
char memory[MEMSIZE];  

struct mem_stack stack = { .top = 395 };
struct freelist *head = NULL;



/***Function definitions****/

int min(int a, int b) {
    return a < b ? a : b;
}

void create_frame(char* func_name , int func_addr )
{
	/* syntax: CF functionname functionaddress*/

	if(strlen(func_name) > 8 )
	{
		printf("Error: function name is too long. Maximum 8 characters allowed.\n");
		return;
	}

	for(int i = 0 ; i< MAXFRAMES ; i++)
	{
		if(stack.frames[i].used == 1)
		{
			if(strcmp(stack.frames[i].name, func_name) == 0)
			{
				printf("Error: function already exists.\n");
				return;
			}
		}
		else 
		{
			if((499-stack.top) >= 300)
			{
				printf("stack overflow, not enough memory available for new function \n");
				return;
			}

			


			stack.frames[i].used = 1;
			char used = '1' ;
			memcpy(&memory[479-(i*21)], &used , 1);
			
			stack.frames[i].number = i;
			char number[4]= "    ";
			itoa(i, number, 10);
			
			memcpy(&memory[(479 -(i*21))+1], &number , 4);
			

			strcpy(stack.frames[i].name, func_name);
			char name[8]= "";
			strcpy(name,func_name);
			memcpy(&memory[(479 - (i*21))+5], &name , 8);

			stack.frames[i].functionaddress = func_addr;
			char funcaddr[4]= "";
			itoa(func_addr, funcaddr, 10);
			memcpy(&memory[(479 - (i*21))+13], &funcaddr , 4);

			

			stack.frames[i].frameaddress = stack.top;
			char frameaddr[4] = "";
			itoa(stack.top, frameaddr, 10);
			memcpy(&memory[(479 - (i*21))+17], &frameaddr , 4);

			currentstacksize += 10;

			stack.top -= 10;
			
			return;
			
		}

		
	}

	printf("cannot create another frame, maximum number of frames have reached\n");
		return;




}

void delete_frame()
{
	/*syntax: DF
	This command deletes the function on top of the stack.*/

	if(currentstacksize < 106)
	{
		printf("Stack is empty \n");
		return;
	}

	for(int i= MAXFRAMES - 1; i > -1; i--)
	{
		if(stack.frames[i].used == 1)
		{
			for(int j= stack.top ; j<stack.frames[i].frameaddress ; j++)
			{
				memory[j] = ' ';
			} 

			currentstacksize -=  ( stack.frames[i].frameaddress - stack.top);

			stack.top = stack.frames[i].frameaddress;

			stack.frames[i].used = 0;
			char used = ' ' ;
			memcpy(&memory[479-(i*21)], &used , 1);

			stack.frames[i].number = 0;
			char number[4]= "    ";
			
			memcpy(&memory[(479 -(i*21))+1], &number , 4);


			strcpy(stack.frames[i].name, "");
			char name[8]= "        ";
			memcpy(&memory[(479 - (i*21))+5], &name , 8);

			stack.frames[i].functionaddress = 0;
			char funcaddr[4] = "    ";
			memcpy(&memory[(479 - (i*21))+13], &funcaddr , 4);


			stack.frames[i].frameaddress = 0;
			char frameaddr[4] = "    ";
			memcpy(&memory[(479 - (i*21))+17], &frameaddr , 4);

			return;

			
		}
	}
	



}


void create_int(char* int_name , int int_value)
{
	/* syntax: CI integername integervalue*/


	if((currentstacksize + 4 > min(300 , 500 - currentheapsize)))
	{
		printf("Error: Stack overflow, cannot create more data on it.\n");
		return;
	}

	int current = -1;
	for(int i= MAXFRAMES - 1; i > -1; i--)
	{
		if(stack.frames[i].used == 1)
		{
			current = i;
			break;
		}
	}

	if(current == -1)
	{
		printf("Error: No frame created on stack.\n");
		return;
	}

	if(( stack.frames[current].frameaddress - stack.top) > 77)
	{
		printf("Error: Frame is full, cannot create more data on it.\n");
		return;
	}

	stack.top -= 4;

	currentstacksize += 4;
	char temp[4]= "    ";
	itoa(int_value, temp, 10);
	memcpy(&memory[stack.top], &temp , sizeof(temp));

	

	return;

}

void create_double(char* double_name , double double_value)
{
	/*syntax: CD doublename doublevalue*/


	if((currentstacksize + 8 > min(300 , 500 - currentheapsize)))
	{
		printf("Error: Stack overflow, cannot create more data on it.\n");
		return;
	}

	int current = -1;
	for(int i= MAXFRAMES-1 ; i > -1; i--)
	{
		if(stack.frames[i].used == 1)
		{
			current = i;
			break;
		}
	}

	if(current == -1)
	{
		printf("Error: No frame created on stack.\n");
		return;
	}

	if(( stack.frames[current].frameaddress - stack.top ) > 73)
	{
		printf("Error: Frame is full, cannot create more data on it.\n");
		return;
	}

	stack.top -= 8;

	currentstacksize += 8;
	char temp[8] = "        ";
	sprintf ( temp, "%f" , double_value);
	memcpy(&memory[stack.top], &temp , sizeof(temp));

	

	return;
}


void create_char(char* char_name , char char_value)
{
	/*syntax: CC charname charvalue */

	if((currentstacksize + 1 > min(300 , 500 - currentheapsize)))
	{
		printf("Error: Stack overflow, cannot create more data on it.\n");
		return;
	}

	int current = -1;
	for(int i= MAXFRAMES - 1; i > -1; i--)
	{
		if(stack.frames[i].used == 1)
		{
			current = i;
			break;
		}
	}

	if(current == -1)
	{
		printf("Error: No frame created on stack.\n");
		return;
	}

	if(( stack.frames[current].frameaddress  -  stack.top ) > 79)
	{
		printf("Error: Frame is full, cannot create more data on it.\n");
		return;
	}

	stack.top -= 1;

	currentstacksize += 1;
	char temp ;
	temp = char_value;
	memcpy(&memory[stack.top], &temp , sizeof(temp));
	 
	return;
	
}

void create_charbuff_heap(const char* charbuff_name , int charbuff_size, struct freelist * free_head , struct allocated * alloc_head)
{
	/*syntax: CH buffername size. 
	*/

        int req_size = charbuff_size + 4;

		if(((currentheapsize)  >= min( 300 , 500 - currentstacksize)))
		{
			printf("Error: heap is full, cannot create more data on it.\n");
			return;
		}



		int current = -1;
		for(int i= MAXFRAMES - 1; i > -1; i--)
		{
			if(stack.frames[i].used == 1)
			{
				current = i;
				break;
			}
		}

		if(current == -1)
		{
			printf("Error: No frame created on stack.\n");
			return;
		}

		if(( stack.frames[current].frameaddress - stack.top) > 77)
		{
			printf("Error: Frame is full, cannot create more data on it.\n");
			return;
		}

		struct freelist * current_freenode;
		current_freenode = find_space(free_head, req_size);

		if(current_freenode == NULL)
		{
			if((currentheapsize + req_size)  >= min( 300 , 500 - currentstacksize))
			{
				printf("Error: heap is full, cannot create more data on it.\n");
				return;
			}
			else
			{
				current_freenode = create_node(currentheapsize, req_size);
				currentheapsize += req_size;
				free_head = insert_node(free_head, current_freenode->start, current_freenode->size);
				free_head = merge_nodes(free_head);
			}

			
		}

		current_freenode = find_space(free_head, req_size);

		int mem_location = current_freenode->start;
		
		current_freenode->start += req_size;
		current_freenode->size -= req_size;

		alloc_head = insert_allocated_node(alloc_head, charbuff_name, mem_location); 
	

		stack.top -= 4;
		currentstacksize += 4;
		char temp[4] = "    ";
		itoa(mem_location, temp, 10);
		memcpy(&memory[stack.top], &temp , 4);

		char temp2[4]= "    ";
		itoa(charbuff_size, temp2, 10);
		memcpy(&memory[mem_location], &temp2 , 4);

		char temp3[charbuff_size];
		for(int i = 0; i < charbuff_size; i++) {
			temp3[i] = 'a' + (rand() % 26);
		}
		memcpy(&memory[mem_location + 4], &temp3 , charbuff_size);

		return;
		

}

void deallocate_heap(char* charbuff_name , struct freelist * free_head , struct allocated * alloc_head)
{
	/*syntax: DH buffername
	*/
	struct allocated * current_allocatednode;
	current_allocatednode = find_allocated_node(alloc_head, charbuff_name);
	// printf("I am outside find \n");
	if(current_allocatednode == NULL)
	{
		printf("Error: the pointer is NULL or already de-allocated.\n");
		return;
	}

	int mem_location = current_allocatednode->startaddress;
	// if (current_allocatednode->next != NULL) {
	// 	int buff_size = current_allocatednode->next->startaddress - mem_location;
	// }

	int buff_size = 0;
	char temp[4] = "    ";
	memcpy(&temp, &memory[mem_location], 4);
	buff_size = atoi(temp);
	buff_size += 4;
	

	alloc_head = delete_allocated(alloc_head, charbuff_name);

	for(int i = 0; i < buff_size; i++) {
		memory[mem_location + i] = ' ';
	}

	free_head = insert_node(free_head, mem_location, buff_size);
	free_head = merge_nodes(free_head);

	return;

}

void show_memory(struct freelist * free_head , struct allocated * alloc_head)
{
	/*syntax: SM
		This command should print the stack and heap snapshots.
	*/

	printf("****************** Frame List ( 500 - 395 ) ******************\n\n");
	printf("Used(1)-Frame Number(4)-Function Name(8)-Function Address(4)-Frame Address(4)\n\n");
	for(int i = 0; i < MAXFRAMES; i++) {
		for(int j = 0; j < 21; j++) {
			printf("%c", memory[479-(i*21)+j]);
		}
		printf("\n");
	}

	int stack_end = min(300 ,(500 - currentstacksize));

	printf("****************** Stack Frames ( 395 - %d ) ******************\n\n",stack_end);

	for(int i = stack_end; i < 396; i++) {
		printf("%c", memory[i]);
	}

	printf("\n\n");



	printf("****************** Heap ( 0 - %d ) ******************\n\n",currentheapsize);

	for(int i = 0; i < currentheapsize; i++) {
		printf("%c", memory[i]);
	}

	printf("\n\n");

	printf(" Free List \n");

	print_free(free_head);
	printf("\n");

	printf(" Allocated List \n");
	print_allocated(alloc_head);
	printf("\n");


}




/***Main function****/


int main () {
	
	struct freelist * head = NULL;
	struct allocated * alloc_head = NULL;

	// Insert nodes at the end
	alloc_head = insert_allocated_node(alloc_head, "", 0);
	head = insert_node(head, 0, 100);


	for(int i = 0; i < 500; i++) 
	{
		memory[i] = ' ';
	}       // Buffer that will emulate stack and heap memory

	printf("Welcome to memory system.\n");
	printf("Press ctrl+c to exit.\n");

	char input[100];
    while (1) {

        printf("Enter command: ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;  // Exit on EOF or input error
        }

        // Remove newline character at end of input
        input[strcspn(input, "\n")] = '\0';

        char command, name[20];
        int address, size;
        double double_value;
        char char_value;

        if (sscanf(input, "CF %s %d", name, &address) == 2) 
		{
            create_frame(name, address);
        } 
		else if (strcmp(input, "DF") == 0) 
		{
            delete_frame();
        } 
		else if (sscanf(input, "CI %s %d", name, &address) == 2) 
		{
            create_int(name, address);
        } 
		else if (sscanf(input, "CD %s %lf", name, &double_value) == 2) 
		{
            create_double(name, double_value);
        } 
		else if (sscanf(input, "CC %s %c", name, &char_value) == 2) 
		{
            create_char(name, char_value);
        } 
		else if (sscanf(input, "CH %s %d", name, &size) == 2) 
		{
            create_charbuff_heap(name, size, head, alloc_head);
        } 
		else if (sscanf(input, "DH %s", name) == 1) 
		{
            deallocate_heap(name, head, alloc_head);
        } 
		else if (strcmp(input, "SM") == 0) 
		{
            show_memory(head, alloc_head);
        } 
		else {
            printf("Unknown command or incorrect syntax.\n");
        }
    }

	free_allocated(alloc_head);
	free_freelist(head);

	return 0;
}




/***Code dump****/

/*	// int currentstacksize = 200;
	// int currentheapsize = 100;
	// char memory[MEMSIZE];         // Buffer that will emulate stack and heap memory
        // char * memory = (char *) malloc (500); // Buffer that will emulate stack and heap memory
	// struct framestatus fs;
	
	head->start = 0;
	head->size = 100;
	head->next = NULL;


	fs.number = 1;
	strcpy(fs.name, "main");
	fs.functionaddress = rand() % 100;
	fs.frameaddress = 395;
	fs.used = 1;

	printf ("frame number: %d\n", fs.number);
	printf ("function name: %s\n", fs.name);
	printf ("function address: %d\n", fs.functionaddress);
	printf ("frame address: %d\n", fs.frameaddress);
	printf ("frame usage: %d\n", fs.used);


	printf ("size of free space on heap: %d\n", head->start);
	printf ("starting address of free heap region: %d\n", head->size);


create_frame("main", 1000);


// printf("stack top: %d\n", stack.top);

create_char("char1", 'a');

// printf("stack top: %d\n", stack.top);

create_int("int1", 2);

// printf("stack top: %d\n", stack.top);

create_frame("mc", 1000);
create_frame("ain", 1000);


// printf("stack top: %d\n", stack.top);

delete_frame();
delete_frame();

// printf("stack top: %d\n", stack.top);
// printf("stack size: %d\n", currentstacksize);

create_int("int1", 10);
create_int("int2", 20);

create_double("double1", 10.5533434343643);
create_double("double1", 10.5);
create_double("double1", 10.5);


// printf("stack top: %d\n", stack.top);
// printf("stack size: %d\n", currentstacksize);

create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);



// printf("stack top: %d\n", stack.top);
// printf("stack size: %d\n", currentstacksize);





create_frame("gain", 1000);

create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);

// printf("stack top: %d\n", stack.top);
// printf("stack size: %d\n", currentstacksize);
delete_frame();
// printf("stack top: %d\n", stack.top);
// printf("stack size: %d\n", currentstacksize);

create_int("int1", 10);
create_int("int1", 10);

show_memory(head, alloc_head);


create_frame("mc", 1000);

// printf("stack top: %d\n", stack.top);
// printf("stack size: %d\n", currentstacksize);

create_frame("ain", 1000);


create_char("char1", 'a');
create_int("int1", 10);
create_double("double1", 10.5);
create_double("double1", 10.5);
create_double("double1", 10.5);

// printf("stack top: %d\n", stack.top);
// printf("stack size: %d\n", currentstacksize);


create_frame("gainsty", 1000);
create_frame("gaingsty", 1000);

printf("stack top: %d\n", stack.top);
printf("stack size: %d\n", currentstacksize);

create_charbuff_heap("myvar", 10 , head, alloc_head);
// print_free(head);
// print_allocated(alloc_head);
create_charbuff_heap("myvar1", 25 , head, alloc_head);
create_charbuff_heap("myvar2", 50 , head, alloc_head);

deallocate_heap("myvar1", head, alloc_head);
deallocate_heap("myvar1", head, alloc_head);
// create_charbuff_heap("myvar3", 2 , head, alloc_head);

printf("heap size: %d\n", currentheapsize);
printf("stack size: %d\n", currentstacksize);

show_memory(head, alloc_head);


// for (int i = 1; i < 500; i++) {
// 	printf("%c", memory[i]);
// }

*/