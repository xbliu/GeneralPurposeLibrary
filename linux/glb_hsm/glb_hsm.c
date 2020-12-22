#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "glb_hsm.h"


void glb_hsm_init(glb_hsm_t *hsm)
{
	super_state_t *cur_state = hsm->cur_state;
    int status;
	super_event_t event;

    //status = (hsm->target->handler)(hsm,&event); /* execute the top-most initial transition */

    /* drill down into the state hierarchy with initial transitions... */
	super_state_t *path[HSM_MAX_NEST_DEPTH_];
	int_fast8_t ip = 0; /* transition entry path index */

	path[0] = hsm->next_state;
	
	event.signal = HSM_EMPTY_SIG;
	SET_STATE_HSM(hsm->next_state,hsm);
	(void)(hsm->next_state->handler)(hsm->next_state,&event);
	while (hsm->next_state != cur_state) {
		++ip;
		path[ip] = hsm->next_state;
		SET_STATE_HSM(hsm->next_state,hsm);
		(void)(hsm->next_state->handler)(hsm->next_state,&event);
	}
	hsm->next_state = path[0];

	/* retrace the entry path in reverse (desired) order... */
	do {
		(void)(path[ip]->on_entry)(path[ip]); /* enter path[ip] */
		--ip;
	} while (ip >= 0);
	
	cur_state = path[0]; /* current state becomes the new source */

    hsm->cur_state = cur_state; /* change the current active src */
    hsm->next_state  = cur_state; /* mark the configuration as stable */
}

int get_super_state(super_state_t *super)
{
	super_event_t event;
	event.signal = HSM_EMPTY_SIG;
	return (super->handler)(super,&event);
}

static int_fast8_t hsm_tran(glb_hsm_t *me, super_state_t *path[HSM_MAX_NEST_DEPTH_])
{
    int_fast8_t ip = -1; /* transition entry path index */
    int_fast8_t iq; /* helper transition entry path index */
    super_state_t *t = path[0];
    super_state_t *s = path[2];
    int status;
	
    /* (a) check source==target (transition to self)... */
    if (s == t) {
		(s->on_exit)(s); /* exit the source */
        ip = 0; /* enter the target */
    } else {
		get_super_state(t); /* find superstate of target */

        t = me->next_state;
        /* (b) check source==target->super... */
        if (s == t) {
            ip = 0; /* enter the target */
        }
        else {
			get_super_state(s); /* find superstate of src */

            /* (c) check source->super==target->super... */
            if (me->next_state == t) {
				(s->on_exit)(s);/* exit the source */
                ip = 0; /* enter the target */
            }
            else {
                /* (d) check source->super==target... */
                if (me->next_state == path[0]) {
                    (s->on_exit)(s); /* exit the source */
                }
                else {
                    /* (e) check rest of source==target->super->super..
                    * and store the entry path along the way
                    */
                    iq = 0; /* indicate that LCA not found */
                    ip = 1; /* enter target and its superstate */
                    path[1] = t;      /* save the superstate of target */
                    t = me->next_state; /* save source->super */

                    /* find target->super->super... */
					status = get_super_state(path[1]);
                    while (status == EVT_STATUS_SUPER) {
                        ++ip;
                        path[ip] = me->next_state; /* store the entry path */
                        if (me->next_state == s) { /* is it the source? */
                            iq = 1; /* indicate that LCA found */

                            /* entry path must not overflow */
							if (ip > HSM_MAX_NEST_DEPTH_) {
								printf("<%s:%d> path overflow\n",__FUNCTION__,__LINE__);
								exit(0);
							}
                            --ip; /* do not enter the source */
                            status = EVT_STATUS_HANDLED; /* terminate loop */
                        }
                         /* it is not the source, keep going up */
                        else {
							status = get_super_state(me->next_state);
                        }
                    }

                    /* the LCA not found yet? */
                    if (iq == 0) {

                        /* entry path must not overflow */
						if (ip > HSM_MAX_NEST_DEPTH_) {
								printf("<%s:%d> path overflow\n",__FUNCTION__,__LINE__);
								exit(0);
						}

                        (s->on_exit)(s); /* exit the source */

                        /* (f) check the rest of source->super
                        *                  == target->super->super...
                        */
                        iq = ip;
                        status = EVT_STATUS_IGNORED; /* LCA NOT found */
                        do {
                            if (t == path[iq]) { /* is this the LCA? */
                                status = EVT_STATUS_HANDLED; /* LCA found */
                                ip = iq - 1; /* do not enter LCA */
                                iq = -1; /* cause termintion of the loop */
                            }
                            else {
                                --iq; /* try lower superstate of target */
                            }
                        } while (iq >= 0);

                        /* LCA not found? */
                        if (status != EVT_STATUS_HANDLED) {
                            /* (g) check each source->super->...
                            * for each target->super...
                            */
                            status = EVT_STATUS_IGNORED; /* keep looping */
                            do {
                                /* exit t unhandled? */
								
                                if ((t->on_exit)(t) == EVT_STATUS_HANDLED)
                                {
//                                    QS_BEGIN_PRE_(QS_QEP_STATE_EXIT, qs_id)
//                                        QS_OBJ_PRE_(me);
//                                        QS_FUN_PRE_(t);
//                                    QS_END_PRE_()
									
									get_super_state(t);
                                }
								
                                t = me->next_state; /* set to super of t */
                                iq = ip;
                                do {
                                    /* is this LCA? */
                                    if (t == path[iq]) {
                                        /* do not enter LCA */
                                        ip = (int_fast8_t)(iq - 1);
                                        iq = -1; /* break out of inner loop */
                                        /* break out of outer loop */
                                        status = EVT_STATUS_HANDLED;
                                    }
                                    else {
                                        --iq;
                                    }
                                } while (iq >= 0);
                            } while (status != EVT_STATUS_HANDLED);
                        }
                    }
                }
            }
        }
    }
    return ip;
}


#if 1
void glb_hsm_dispatch(glb_hsm_t * hsm, super_event_t *sevt)
{
	super_state_t *cur_state = hsm->cur_state;
    super_state_t *next_state = NULL;
	super_state_t *state = NULL;
	super_event_t event;
    int status;
    int_fast8_t iq; /* helper transition entry path index */

    /** @pre the current state must be initialized and
    * the state configuration must be stable
    */
//    Q_REQUIRE_ID(400, (t != Q_STATE_CAST(0))
//                      && (t == hsm->target));
//
    /* process the event hierarchically... */
    do {
        next_state = hsm->next_state;
        status = (*next_state->handler)(next_state,sevt); /* invoke state handler s */
		
        if (status == EVT_STATUS_HANDLED) { /* unhandled due to a guard? */
			/* save the original signal */
			/* find the superstate */
			status = get_super_state(next_state);
            /* invoke state handler s */
			/* restore the original signal */
        }
    } while (status == EVT_STATUS_SUPER);

    /* transition taken? */
    if (status >= EVT_STATUS_TRAN) {
        super_state_t *path[HSM_MAX_NEST_DEPTH_]; /* transition entry path */
        int_fast8_t ip; /* transition entry path index */

        path[0] = hsm->next_state; /* save the target of the transition */
        path[1] = next_state;
        path[2] = cur_state;

        /* exit current state to transition source s... */
        for (; next_state != cur_state; next_state = hsm->next_state) {
			/* find superstate of t */

            /* take the exit action and check if it was handled? */
            if ((next_state->on_exit)(next_state) == EVT_STATUS_HANDLED) {
				get_super_state(next_state);/* find superstate of t */
            }
        }

        ip = hsm_tran(hsm, path); /* take the state transition */

        /* retrace the entry path in reverse (desired) order... */
        //Q_SIG(hsm) = Q_ENTRY_SIG;
        for (; ip >= 0; --ip) {
			state = path[ip];
            (void)(state->on_entry)(state); /* enter path[ip] */
        }
        next_state = path[0];      /* stick the target into register */
        hsm->next_state = next_state; /* update the current state */
		
        /* drill into the target hierarchy... */
		ip = 0;

		path[0] = hsm->next_state;
		get_super_state(hsm->next_state);/* find the superstate */
		while (hsm->next_state != next_state) {
			++ip;
			path[ip] = hsm->next_state;
			get_super_state(hsm->next_state);/* find the superstate */
		}
		hsm->next_state = path[0];

		/* entry path must not overflow */
		//Q_ASSERT_ID(410, ip < QHSM_MAX_NEST_DEPTH_);

		/* retrace the entry path in reverse (correct) order... */
		//Q_SIG(hsm) = Q_ENTRY_SIG;
		do {
			state = path[ip];
			(void)(state->on_entry)(state); /* enter path[ip] */
			--ip;
		} while (ip >= 0);

		next_state = path[0];
    }

    hsm->cur_state = next_state; /* change the current active state */
    hsm->next_state  = next_state; /* mark the configuration as stable */
}

#endif

int hsm_root_handler(super_state_t *super, super_event_t *sevt) {
    (void)super; /* suppress the "unused parameter" compiler warning */
    return EVT_STATUS_IGNORED; /* the top state ignores all events */
}

static super_state_t m_root_state = {
	.hsm = NULL,
	.on_entry = NULL,
	.on_exit = NULL,
	.handler = hsm_root_handler,
};

super_state_t *get_root_state()
{
	return &m_root_state;
}

void glb_hsm_ctor(glb_hsm_t *hsm, super_state_t *initial) {
    static hsm_vtable_t vtable = { /* hsm virtual table */
        &glb_hsm_init,
        //&QHsm_dispatch_
    };
	
    hsm->vtable  = &vtable;
    hsm->cur_state = get_root_state();
    hsm->next_state  = initial;
}

static glb_hsm_t m_glb_hsm;
void hsm_init(super_state_t *state)
{
	glb_hsm_ctor(&m_glb_hsm,state);
	m_glb_hsm.vtable->init(&m_glb_hsm);
}

