#include <stdio.h>
#include <stdlib.h>

#include "singly_linked_list.h"

static int list_show(void *data)
{
	fprintf(stderr," %ld ",(long)data);
	return 0;
}

static int integer_compare(void *src, void *dest)
{
	return ((long)src == (long)dest);
}

int main(int argc, char *argv[])
{
	singly_link_list_t *list = singly_link_list_new(INSERT_TYPE_TAIL,NULL);
	singly_link_list_add(list,(void *)1);
	singly_link_list_add(list,(void *)2);
	singly_link_list_add(list,(void *)3);
	singly_link_list_add(list,(void *)2);
	singly_link_list_add(list,(void *)4);
	
	
	dump_singly_link_list(list,list_show);
	
	singly_link_list_del_bydata(list,(void *)4,integer_compare);
	singly_link_list_del(list,list->head);
	
	singly_link_list_add(list,(void *)5);
	dump_singly_link_list(list,list_show);
	
	singly_link_list_free(list);
	
	return 0;
}


