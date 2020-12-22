#include "common_level.h"

int	level2_msg_handler(super_state_t *super, super_event_t *sevt)
{
	int status = EVT_STATUS_UNHANDLE;
	switch(sevt->signal) {
		case LEVEL_START_SIG:
			status = EVT_STATUS_HANDLED;
			break;
		case LEVEL_STOP_SIG:
			status = EVT_STATUS_HANDLED; // for reserver tran
			break;
		default:
			status = EVT_SUPER(super->hsm,get_level1_state());
			break;
	}
	
	return status;
}

static level_state_t m_level2_state = {
	.state = {
		.on_entry = level_entry,
		.on_exit = level_exit,
		.handler = level2_msg_handler,
	},
	.name = "level 2 state",
};

super_state_t *get_level2_state()
{
	return &m_level2_state.state;
}

