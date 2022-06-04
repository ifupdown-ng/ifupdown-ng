#ifndef LIBIFUPDOWN_WAITIF_H__GUARD
#define LIBIFUPDOWN_WAITIF_H__GUARD

#include <stdbool.h>
#include <semaphore.h>

#include "libifupdown/dict.h"

struct waitif_iface {
	int target_state;
	sem_t sema;
};

extern bool lif_waitif_init(void);
extern bool lif_waitif_setup(struct waitif_iface *iface, const char *name);
extern bool lif_waitif_wait(struct waitif_iface *iface, unsigned timeout);

#endif
