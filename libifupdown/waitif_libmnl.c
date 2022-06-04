#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>

#include <net/if.h>
#include <libmnl/libmnl.h>
#include <linux/if.h>
#include <linux/if_link.h>
#include <linux/rtnetlink.h>

#include "libifupdown/waitif.h"

struct waitif_listener {
	pthread_t pthread;
	pthread_mutex_t mtx;
	struct lif_dict ifs;
};

/* Only one listener will be needed for ifupdown, thus global. */
static struct waitif_listener listener;

static int
netlink_cb(const struct nlmsghdr *nlh, void *arg)
{
	struct waitif_listener *ln = (struct waitif_listener *)arg;

	struct ifinfomsg *ifm;
	ifm = mnl_nlmsg_get_payload(nlh);

	char ifname[IF_NAMESIZE];
	if (!if_indextoname(ifm->ifi_index, ifname)) {
		fprintf(stderr, "ifupdown: if_indextoname failed for interface %d: %s\n", ifm->ifi_index, strerror(errno));
		return MNL_CB_OK; /* continue processing other ifaces */
	}

	pthread_mutex_lock(&ln->mtx);
	struct lif_dict_entry *entry;
	if (!(entry = lif_dict_find(&ln->ifs, ifname)))
		return MNL_CB_OK; /* no link state handling requested */

	struct waitif_iface *iface;
	iface = (struct waitif_iface *)entry->data;
	if (ifm->ifi_flags & iface->target_state)
		sem_post(&iface->sema);
	lif_dict_delete(&ln->ifs, ifname);
	pthread_mutex_unlock(&ln->mtx);

	return MNL_CB_OK;
}

static void *
netlink_loop(void *arg)
{
	char buf[MNL_SOCKET_BUFFER_SIZE];

	struct waitif_listener *ln;
	ln = (struct waitif_listener *)arg;

	struct mnl_socket *nl;
	nl = mnl_socket_open(NETLINK_ROUTE);
	if (nl == NULL)
		goto err0;
	if (mnl_socket_bind(nl, RTMGRP_LINK, MNL_SOCKET_AUTOPID) < 0)
		goto err1;

	ssize_t ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, (size_t)ret, 0, 0, netlink_cb, (void *)ln);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}
	if (ret == -1)
		goto err1;

err1:
	mnl_socket_close(nl);
err0:
	fprintf(stderr, "ifupdown: netlink_loop failed %s\n", strerror(errno));
	return NULL;
}

bool
lif_waitif_init(void)
{
	lif_dict_init(&listener.ifs);
	if (pthread_mutex_init(&listener.mtx, NULL))
		return false;
	if (pthread_create(&listener.pthread, NULL, netlink_loop, &listener))
		return false;

	return true;
}

bool
lif_waitif_setup(struct waitif_iface *iface, const char *name)
{
	iface->target_state = IFF_RUNNING; /* XXX: Make this configurable? */
	if (sem_init(&iface->sema, 1, 0))
		return false;

	if (pthread_mutex_lock(&listener.mtx))
		return false;
	lif_dict_add(&listener.ifs, name, iface);
	if (pthread_mutex_unlock(&listener.mtx))
		return false;

	return true;
}

bool
lif_waitif_wait(struct waitif_iface *iface, unsigned timeout)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_REALTIME, &ts))
		return false;
	ts.tv_sec += timeout;
	if (sem_timedwait(&iface->sema, &ts) == -1)
		return false;

	return true;
}
