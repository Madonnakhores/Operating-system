#include <inc/lib.h>

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
uint32 start_address = USER_HEAP_START;

struct user_heap
{
	uint32 start_ptr;
	int size;
	uint32 end_ptr;

}pages[(USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE];

struct free_pages
{
	uint32 start_ptr;
	int size;
	int index;
}free_spaces[200];

int count = 0;
int numberOFfree_spaces = 0;

void* malloc(uint32 size)
{
	int new_size = ROUNDUP(size , PAGE_SIZE);
	if(new_size > USER_HEAP_MAX - USER_HEAP_START)
			return NULL;
	int no_frames = new_size / PAGE_SIZE;
	//TODO: [PROJECT 2021 - [2] User Heap] malloc() [User Side]
	// Write your code here, remove the panic and write your code
	int frames_sum = 0;
	uint32 address = 0;
	int index;
	int best_fit = USER_HEAP_MAX - USER_HEAP_START;
	bool best_srategy_flag = 0;
	if(count == 0)
	{
		address = start_address;
		for(int i = 0; i < no_frames; i++)
		{
			pages[i].start_ptr = start_address;
			pages[i].size = new_size;
			pages[i].end_ptr = start_address + new_size;
		}
		sys_allocateMem(address , size);
		count++;
		start_address += PAGE_SIZE * no_frames;
		return (void*)address;
	}
	else
	{
		for(int i = 0; i < numberOFfree_spaces; i++)
		{

			if((free_spaces[i].size >= new_size) && (free_spaces[i].size < best_fit))
			{
				best_fit = free_spaces[i].size;

				address = free_spaces[i].start_ptr;
				index = free_spaces[i].index;
				best_srategy_flag = 1;
			}

		}
		if((!best_srategy_flag))
		{
			address = start_address;
			index =(address -  USER_HEAP_START) / PAGE_SIZE;
		}
	}

		if((address == 0) || (address + new_size > USER_HEAP_MAX))
			return NULL;
		else
		{
			sys_allocateMem(address , size);
			for(int i = index; i < index + no_frames; i++)
			{
				pages[i].start_ptr = address;
				pages[i].size = new_size;
				pages[i].end_ptr = address + new_size;
			}
		}

	if(best_srategy_flag)
	{
		for(int i = 0 ; i < numberOFfree_spaces; i++)
		{
			if(index == free_spaces[i].index)
			{
				if(new_size == free_spaces[i].size)
				{
					free_spaces[i].index = 0;
					free_spaces[i].size = 0;
					free_spaces[i].start_ptr = 0;
					numberOFfree_spaces --;
				}
				else if(new_size < free_spaces[i].size)
				{
					free_spaces[i].index += (new_size / PAGE_SIZE);
					free_spaces[i].size -= new_size ;
					free_spaces[i].start_ptr += new_size;
				}
				break;
			}
		}
	}
	//This function should find the space of the required range
	//using the BEST FIT strategy
	//refer to the project presentation and documentation for details
	if(!best_srategy_flag)
		start_address += new_size;
	count++;
	return (void*)address;

}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2021 - [2] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code
	int size;
	int index;
	for(int i = 0; i < (USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE; i++)
	{
		if(virtual_address == (void*)pages[i].start_ptr)
		{
			size = pages[i].size;
			index = i;
			break;
		}
	}
	sys_freeMem((uint32)virtual_address , size);
	for(int i = 0; i < numberOFfree_spaces + 1; i++)
	{
		if(free_spaces[i].start_ptr +free_spaces[i].size == pages[index].start_ptr)
		{
			free_spaces[i].size += size;
			break;
		}
		if(free_spaces[i].size == 0)
		{
			free_spaces[i].size = size;
			free_spaces[i].index = index;
			free_spaces[i].start_ptr = pages[index].start_ptr;
			numberOFfree_spaces++;
			break;
		}
	}

	if((uint32)(virtual_address + size) == start_address)
			start_address -= size;
	count--;
	//you should get the size of the given allocation using its address

	//refer to the project presentation and documentation for details
}

//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("this function is not required...!!");
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("this function is not required...!!");
	return 0;
}

void sfree(void* virtual_address)
{
	panic("this function is not required...!!");
}

void *realloc(void *virtual_address, uint32 new_size)
{
	panic("this function is not required...!!");
	return 0;
}

void expand(uint32 newSize)
{
	panic("this function is not required...!!");
}
void shrink(uint32 newSize)
{
	panic("this function is not required...!!");
}

void freeHeap(void* virtual_address)
{
	panic("this function is not required...!!");
}
