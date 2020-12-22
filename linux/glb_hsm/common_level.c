#include <stdio.h>
#include "common_level.h"


int level_entry(super_state_t *super)
{
	level_state_t *level_state = container_of(super,level_state_t,state);
	printf("<%s:%d>  name:%s #########\n",__FUNCTION__,__LINE__,level_state->name);
	return 0;
}

int level_exit(super_state_t *super)
{
	level_state_t *level_state = container_of(super,level_state_t,state);
	printf("<%s:%d>  name:%s #########\n",__FUNCTION__,__LINE__,level_state->name);
	return 0;
}

