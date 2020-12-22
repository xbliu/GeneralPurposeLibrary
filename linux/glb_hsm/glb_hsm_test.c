#include <stdio.h>
#include <stdlib.h>

#include "common_level.h"

int main(int argc, char *argv[])
{
	hsm_init(get_level3_state());
	
	return 0;
}
