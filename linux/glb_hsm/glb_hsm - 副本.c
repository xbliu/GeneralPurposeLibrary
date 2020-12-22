#include "glb_hsm.h"


int hsm_top_handler(void const * const me, super_event_t *sevt) {
    (void)me; /* suppress the "unused parameter" compiler warning */
    return Q_RET_IGNORED; /* the top state ignores all events */
}

void hsm_ctor(super_hsm_t * const me, super_state_t *initial) {
    static hsm_vtable_t const vtable = { /* QHsm virtual table */
        &glb_hsm_init,
        //&QHsm_dispatch_
    };
    me->vtable  = &vtable;
    me->src = get_top_state();
    me->target  = initial;
}

void glb_hsm_init(super_hsm_t * me)
{
	super_state_t src = me->src;
    int status;

    /** @pre the virtual pointer must be initialized, the top-most initial
    * transition must be initialized, and the initial transition must not
    * be taken yet.
    */
//    Q_REQUIRE_ID(200, (me->vptr != (QHsmVtable const *)0)
//                      && (me->target != Q_STATE_CAST(0))
//                      && (t == Q_STATE_CAST(&QHsm_top)));

    status = (*me->target->handler)(me); /* execute the top-most initial transition */

    /* the top-most initial transition must be taken */
 //   Q_ASSERT_ID(210, r == Q_RET_TRAN);

    /* drill down into the state hierarchy with initial transitions... */
    do {
        super_state_t path[QHSM_MAX_NEST_DEPTH_];
        int_fast8_t ip = 0; /* transition entry path index */

        path[0] = me->target;
        Q_SIG(me) = QEP_EMPTY_SIG_;
        (void)(*me->target->handler)(me);
        while (me->target != t) {
            ++ip;
            path[ip] = me->target;
            (void)(*me->target->handler)(me);
        }
        me->target = path[0];

        /* retrace the entry path in reverse (desired) order... */
        do {
            (void)(path[ip]->on_entry)(me); /* enter path[ip] */
            --ip;
        } while (ip >= 0);

        src = path[0]; /* current state becomes the new source */
    } while (status == Q_RET_TRAN);

    me->src = src; /* change the current active src */
    me->target  = src; /* mark the configuration as stable */
}

#if 0
void glb_hsm_dispatch(super_hsm_t * me, super_event_t *sevt)
{
	super_state_t t = me->src;
    super_state_t s;
    int status;
    int_fast8_t iq; /* helper transition entry path index */

    /** @pre the current state must be initialized and
    * the state configuration must be stable
    */
//    Q_REQUIRE_ID(400, (t != Q_STATE_CAST(0))
//                      && (t == me->target));
//
    /* process the event hierarchically... */
    do {
        s = me->target;
        status = (*s->handler)(me); /* invoke state handler s */

        if (status == Q_RET_UNHANDLED) { /* unhandled due to a guard? */
            iq = (int_fast8_t)Q_SIG(me); /* save the original signal */
            Q_SIG(me) = QEP_EMPTY_SIG_; /* find the superstate */
            status = (s->handler)(me); /* invoke state handler s */
            Q_SIG(me) = (QSignal)iq; /* restore the original signal */
        }
    } while (status == Q_RET_SUPER);

    /* transition taken? */
    if (status >= Q_RET_TRAN) {
        super_state_t path[QHSM_MAX_NEST_DEPTH_]; /* transition entry path */
        int_fast8_t ip; /* transition entry path index */

        path[0] = me->target; /* save the target of the transition */
        path[1] = t;
        path[2] = s;

        /* exit current state to transition source s... */
        for (; t != s; t = me->target) {
			/* find superstate of t */

            /* take the exit action and check if it was handled? */
            if ((t->on_exit)(me) == Q_RET_HANDLED) {
                Q_SIG(me) = QEP_EMPTY_SIG_;
                (void)(*t->handler)(me); /* find superstate of t */
            }
        }

        ip = QHsm_tran_(me, path); /* take the state transition */

        /* retrace the entry path in reverse (desired) order... */
        //Q_SIG(me) = Q_ENTRY_SIG;
        for (; ip >= 0; --ip) {
            (void)(path[ip]->on_entry)(me); /* enter path[ip] */
        }
        t = path[0];      /* stick the target into register */
        me->target = t; /* update the current state */

        /* drill into the target hierarchy... */
		ip = 0;

		path[0] = me->target;
		Q_SIG(me) = QEP_EMPTY_SIG_;
		(void)(me->target->handler)(me); /* find the superstate */
		while (me->target != t) {
			++ip;
			path[ip] = me->target;
			(void)(*me->target->handler)(me); /* find the superstate */
		}
		me->target = path[0];

		/* entry path must not overflow */
		//Q_ASSERT_ID(410, ip < QHSM_MAX_NEST_DEPTH_);

		/* retrace the entry path in reverse (correct) order... */
		//Q_SIG(me) = Q_ENTRY_SIG;
		do {
			(void)(path[ip]->on_entry)(me); /* enter path[ip] */
			--ip;
		} while (ip >= 0);

		t = path[0];
    }

    me->src = t; /* change the current active state */
    me->target  = t; /* mark the configuration as stable */
}

#endif


static super_state_t top_state = {
	.on_entry = NUll,
	.on_exit = NULL,
	.handler = hsm_top_handler,
};

super_state_t *get_top_state()
{
	return &top_state;
}
