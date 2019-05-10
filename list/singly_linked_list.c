#include "singly_linked_list.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	void *src;
	list_compare_func internal_compare;
	list_compare_func do_compare;
	int is_more; /*delete the first one found or all*/
} list_compare_t;


static int list_node_compare(void *src, void *dest)
{
	list_compare_t *compare = (list_compare_t *)src;
	
	return  ((compare->src == dest) ? 1 : 0 );
}

static int list_data_compare(void *src, void *dest)
{
	list_compare_t *compare = (list_compare_t *)src;
	node_t *ptr = (node_t *)dest;
	
	return  compare->do_compare(compare->src,ptr->data);
}

static int singly_link_list_common_del(singly_link_list_t *list, list_compare_t *compare)
{
	node_t **ptr = NULL;
	
	if(!list) {
        return -1;
	}
	
    for(ptr = &list->head; *ptr; ptr = &(*ptr)->next) {
more_compare:		
		if (!compare->internal_compare(compare,*ptr)) {
			continue;
		}
		
		node_t *tmp = *ptr;
		if (list->free_func) {
			list->free_func((*ptr)->data);
		}
		*ptr = (*ptr)->next;
		
		if (INSERT_TYPE_TAIL == list->type) {
			if(!*ptr) {
				list->tail = ptr;
			}
		}
		
		free(tmp);
		
		if (compare->is_more && (*ptr)) {
			goto more_compare;
		} else {
			break;
		}
	}
	
	return 0;
}


singly_link_list_t *singly_link_list_new(insert_type_e type, list_free_func free_func)
{
    singly_link_list_t *list;
    list = malloc(sizeof(singly_link_list_t));
    if(!list) {
        return NULL;
	}
    list->head = NULL;
    list->tail = &list->head;
	list->type = type;
	list->free_func = free_func;
    return list;
}

int singly_link_list_add(singly_link_list_t *list, void *data)
{
    node_t *ptr = malloc(sizeof(node_t));
    if(!list) {
        return -1;
	}
	
    ptr->data = data;
	if (INSERT_TYPE_TAIL == list->type) {
		ptr->next = NULL;
		*list->tail = ptr;
		list->tail = &ptr->next;
	} else {
		ptr->next = *list->tail;
		list->head = ptr;
		list->tail = &list->head;
	}
	
    return 0;
}

int singly_link_list_del(singly_link_list_t *list, node_t *cur)
{
	list_compare_t compare;
	compare.src = cur;
	compare.internal_compare = list_node_compare;
	compare.do_compare = NULL;
	compare.is_more = 0;
	return singly_link_list_common_del(list,&compare);
}

int singly_link_list_del_bydata(singly_link_list_t *list, void *data, list_compare_func do_compare)
{
	list_compare_t compare;
	compare.src = data;
	compare.internal_compare = list_data_compare;
	compare.do_compare = do_compare;
	compare.is_more = 1;
	return singly_link_list_common_del(list,&compare);
}

void singly_link_list_free(singly_link_list_t *list)
{
    if(!list) {
        return;
	}
	
    while(list->head) {
        node_t *ptr = list->head;
        if(list->free_func) {
            list->free_func(ptr->data);
		}
		
        list->head = ptr->next;
        free(ptr);
    }
    free(list);
}

void dump_singly_link_list(singly_link_list_t *list, list_show_func do_show)
{
	node_t **ptr = NULL;
	for(ptr = &list->head; *ptr; ptr = &(*ptr)->next) {
		if (do_show) {
			do_show((*ptr)->data);
		}
	}
	fprintf(stderr,"\n");
}



