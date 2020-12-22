#ifndef COMMON_LEVLE_H
#define COMMON_LEVLE_H

#include "glb_hsm.h"

#define LEVEL_START_SIG (HSM_EMPTY_SIG + 1)
#define LEVEL_STOP_SIG (LEVEL_START_SIG + 1)

typedef struct {
	super_state_t state;
	char *name;
} level_state_t;

int level_entry(super_state_t *super);
int level_exit(super_state_t *super);

super_state_t *get_level1_state();
super_state_t *get_level2_state();
super_state_t *get_level3_state();


#endif