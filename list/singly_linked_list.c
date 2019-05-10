#include "singly_linked_list.h"

typedef struct {
	node_t node;
	void *data;
} template_t;

singly_linked_list_t *single_list_new(insert_type_e type)
{
	singly_linked_list_t *list;
    list = malloc(sizeof(singly_linked_list_t));
    if(!list) {
        return NULL;
	}
	
	INIT_SINGLY_LINKED_LIST(list,type);
	
	return list;
}

int single_list_add(singly_linked_list_t *list, void *data)
{
	template_t *ptr = malloc(sizeof(template_t));
    if(!list) {
        return -1;
	}
	
	ptr->data = data;
	
	singly_linked_list_add(list,(node_t *)ptr);
	
	return 0;
}

int single_list_del(singly_linked_list_t *list, template_t *template)
{
	singly_linked_list_del(list,(node_t *)template);
	
	return 0;
}

int single_list_free(singly_linked_list_t *list)
{
	node_t *ptr = NULL;
	while (list->head) {
		ptr = list->head;
		list->head = ptr->next;
		free(ptr);
	}
	
	free(list);
}


void dump_template(singly_linked_list_t *list, list_show_func do_show)
{
	template_t *ptr = NULL;
	list_for_each_entry(ptr, list->head, node) {
		if (do_show) {
			do_show(ptr->data);
		}
	}
	fprintf(stderr,"\n");
}
