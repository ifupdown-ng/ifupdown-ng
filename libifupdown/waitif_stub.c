#include "libifupdown/waitif.h"

bool
lif_waitif_init(void)
{
	return true;
}

bool
lif_waitif_setup(struct waitif_iface *iface, const char *name)
{
	(void) iface;
	(void) name;

	return true;
}

bool
lif_waitif_wait(struct waitif_iface *iface, unsigned timeout)
{
	(void) iface;
	(void) timeout;

	return true;
}
