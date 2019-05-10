#ifndef SINGLY_LINKED_LIST_H_
#define SINGLY_LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>

typedef int (*list_show_func)(void *data);

/*
distinguish whether the new node is inserted from the list header or the end of the list.
*/
typedef enum {
	INSERT_TYPE_HEAD=0, /*can be used for stack*/
	INSERT_TYPE_TAIL, /*can be used for fifo*/
} insert_type_e;

typedef struct node {
	struct node *next;
} node_t;

typedef struct {
	node_t *head;
	node_t **tail;
	insert_type_e type;
} singly_linked_list_t;


#define INIT_SINGLY_LINKED_LIST(ptr,type) \
  do { \
		(list)->head = NULL; \
		(list)->tail = &(list)->head; \
		(list)->type = (type); \
  } while(0)

/**
 * singly_linked_list_add - add node to list
 * @list:	list head pointer.
 * @new:	node to be added
 */
static inline void singly_linked_list_add(singly_linked_list_t *list, node_t *new)
{
	if (INSERT_TYPE_TAIL == list->type) {
		new->next = NULL;
		*list->tail = new;
		list->tail = &new->next;
	} else {
		new->next = *list->tail;
		list->head = new;
		list->tail = &list->head;
	}
}

/**
 * singly_linked_list_del - deletes node from list
 * @list:	list head pointer.
 * @new:	node to be deleted
 */
static inline void singly_linked_list_del(singly_linked_list_t *list, node_t *new)
{
	node_t *tmp = NULL;
	node_t **ptr = NULL;
	
	for(ptr = &list->head; *ptr; ptr = &(*ptr)->next) {
		tmp = *ptr;
		if (tmp != new) {
			continue;
		}
		
		*ptr = (*ptr)->next;
		if (INSERT_TYPE_TAIL == list->type) {
			if(!*ptr) {
				list->tail = ptr;
			}
		}
		
		free(tmp);
	}
}

/**
 * container_of - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
   
/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head); pos; pos = pos->next)
   
/**
 * list_for_each_entry	- iterate over list of given type
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = container_of((head), typeof(*pos), member);	\
	     pos; 					\
	     pos = container_of(pos->member.next, typeof(*pos), member))   

#endif

