#include <stdio.h>
#include <stdlib.h>

#include "singly_linked_list.h"

static int list_show(void *data)
{
	fprintf(stderr," %ld ",(long)data);
	return 0;
}

int main(int argc, char *argv[])
{
	singly_linked_list_t *list = single_list_new(INSERT_TYPE_TAIL);
#if 1	
	single_list_add(list,(void *)1);
	single_list_add(list,(void *)2);
	single_list_add(list,(void *)3);
	single_list_add(list,(void *)2);
	single_list_add(list,(void *)4);
	dump_template(list,list_show);
	
	single_list_del(list,list->head);
	single_list_add(list,(void *)5);
	dump_template(list,list_show);
#endif				  
	
	single_list_free(list);
	
	return 0;
}


