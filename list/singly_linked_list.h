#ifndef SINGLY_LINKED_LIST_H_
#define SINGLY_LINKED_LIST_H_

/*
distinguish whether the new node is inserted from the list header or the end of the list.
*/
typedef enum {
	INSERT_TYPE_HEAD=0, /*can be used for stack*/
	INSERT_TYPE_TAIL, /*can be used for fifo*/
} insert_type_e;

typedef int (*list_free_func)(void *data);
typedef int (*list_show_func)(void *data);
typedef int (*list_compare_func)(void *src, void *dest);

typedef struct node {
	struct node *next;
	void *data;
} node_t;

typedef struct {
	node_t *head;
	node_t **tail;
	insert_type_e type;
	list_free_func free_func;
} singly_link_list_t;


singly_link_list_t *singly_link_list_new(insert_type_e type, list_free_func free_func);
int singly_link_list_add(singly_link_list_t *list, void *data);
int singly_link_list_del(singly_link_list_t *list, node_t *cur);
int singly_link_list_del_bydata(singly_link_list_t *list, void *data, list_compare_func do_compare);
void singly_link_list_free(singly_link_list_t *list);
void dump_singly_link_list(singly_link_list_t *list, list_show_func do_show);


#endif



