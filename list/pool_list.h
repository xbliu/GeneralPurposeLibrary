#ifndef _POOL_LIST_H_
#define _POOL_LIST_H_

#include "klist.h"

typedef struct {
	struct list_head list;
	void *addr;
} pool_elem_t;

typedef struct {
	struct list_head slub;
	struct list_head free;
    struct list_head available;
	
	int free_num;
	int cur_slub;
	unsigned int max_slub;
	
	int elem_size;
	int prealloc_num;
} pool_list_t;


#endif
