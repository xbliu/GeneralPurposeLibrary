#ifndef GLB_HSM_H
#define GLB_HSM_H

#define HSM_MAX_NEST_DEPTH_ (4)

#define HSM_EMPTY_SIG (0)
//#define HSM_INIT_SIG (1) //default sub state


#ifndef offsetof
#define offsetof(type, member) ((unsigned int) &((type *)0)->member)
#endif

#ifndef container_of
#define container_of(ptr, type, member)  ((type *)( (char *)(ptr) - offsetof(type,member) ))
#endif

typedef struct {
	int signal; /*signal of event*/
} super_event_t;

typedef enum {
	EVT_STATUS_SUPER = 0,
	EVT_STATUS_HANDLED,
	EVT_STATUS_IGNORED,
	EVT_STATUS_UNHANDLE,
	EVT_STATUS_TRAN,
} event_status_e;

//#define HSM_UPCAST(ptr_) ((QHsm *)(ptr_))

#define EVT_SUPER(me,super)  \
    (((me))->next_state = (super), EVT_STATUS_SUPER)

#define SET_STATE_HSM(state,hsm) \
	(state)->hsm = (hsm)
	
#define GET_STATE(name) \
	super_state_t *get_#name() \
	{ \
		return &m_#name; \
	}

typedef struct glb_hsm glb_hsm_t;
typedef struct super_state super_state_t;
	
typedef struct hsm_vtable {
    void (*init)(glb_hsm_t * me);
    void (*dispatch)(glb_hsm_t * me, super_event_t *sevt);
} hsm_vtable_t;

typedef struct glb_hsm {
	hsm_vtable_t *vtable;
	super_state_t *cur_state;
	super_state_t *next_state;
} glb_hsm_t;

typedef int (*action_func)(super_state_t *super);
typedef int (*msg_handler_func)(super_state_t *super, super_event_t *sevt);
typedef struct super_state {
	glb_hsm_t *hsm;
	action_func on_entry;
	action_func on_exit;
	msg_handler_func handler;
} super_state_t;


void hsm_init(super_state_t *state);
super_state_t *get_root_state();

#endif
