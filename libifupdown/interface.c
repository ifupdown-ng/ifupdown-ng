/*
 * libifupdown/interface.c
 * Purpose: interface management
 *
 * Copyright (c) 2020 Ariadne Conill <ariadne@dereferenced.org>
 * Copyright (c) 2020 Maximilian Wilhelm <max@sdn.clinic>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include "libifupdown/interface.h"
#include "libifupdown/config-file.h"

bool
lif_address_parse(struct lif_address *address, const char *presentation)
{
	char buf[512], *netmask_p;

	strlcpy(buf, presentation, sizeof buf);

	address->domain = strchr(buf, ':') != NULL ? AF_INET6 : AF_INET;

	netmask_p = strrchr(buf, '/');
	if (netmask_p != NULL)
	{
		*netmask_p++ = '\0';
		address->netmask = strtol(netmask_p, NULL, 10);
	}
	else
		address->netmask = 0;

	return !!inet_pton(address->domain, buf, address->addr_buf);
}

bool
lif_address_unparse(const struct lif_address *address, char *buf, size_t buflen, bool with_netmask)
{
	char workbuf[512] = {};

	if (!inet_ntop(address->domain, address->addr_buf, workbuf, sizeof workbuf))
		return false;

	if (!with_netmask || !address->netmask)
	{
		strlcpy(buf, workbuf, buflen);
		return true;
	}

	snprintf(buf, buflen, "%s/%zu", workbuf, address->netmask);
	return true;
}

static inline size_t
count_set_bits(const char *netmask)
{
        /* netmask set to CIDR length */
	if (strchr(netmask, '.') == NULL)
		return strtol(netmask, NULL, 10);

	size_t r = 0;
	struct in_addr in;

	if (inet_pton(AF_INET, netmask, &in) == 0)
		return r;

	/* take the IP, put it in host endian order, and
	 * flip it so that all the set bits are set to the right.
	 * then we can simply count down from 32 and right-shift
	 * until the bit field is all zero.
	 */
	unsigned int bits = htonl(in.s_addr);
	for (bits = ~bits, r = 32; bits; bits >>= 1, r--)
		;

	return r;
}

bool
lif_address_format_cidr(const struct lif_interface *iface, struct lif_dict_entry *entry, char *buf, size_t buflen)
{
	struct lif_address *addr = entry->data;
	size_t orig_netmask = addr->netmask;

	if (!addr->netmask)
	{
		/* if netmask is not set, default to 255.255.255.0, ifupdown does so too */
		addr->netmask = 24;

		struct lif_dict_entry *entry = lif_dict_find(&iface->vars, "netmask");
		if (entry != NULL)
			addr->netmask = count_set_bits(entry->data);
	}

	if (!lif_address_unparse(addr, buf, buflen, true))
	{
		addr->netmask = orig_netmask;
		return false;
	}

	addr->netmask = orig_netmask;
	return true;
}

void
lif_interface_init(struct lif_interface *interface, const char *ifname)
{
	memset(interface, '\0', sizeof *interface);

	interface->ifname = strdup(ifname);

	lif_interface_use_executor(interface, "link");

	/* keep the 'vlan' executor as a config hint for backwards compatibility */
	if (strchr(ifname, '.') != NULL)
		lif_interface_use_executor(interface, "vlan");
}

bool
lif_interface_address_add(struct lif_interface *interface, const char *address)
{
	struct lif_address *addr = calloc(1, sizeof *addr);

	if (!lif_address_parse(addr, address))
	{
		free(addr);
		return false;
	}

	lif_interface_use_executor(interface, "static");

	lif_dict_add(&interface->vars, "address", addr);

	return true;
}

void
lif_interface_address_delete(struct lif_interface *interface, const char *address)
{
	struct lif_node *iter, *iter_next;
	struct lif_address addr;

	if (!lif_address_parse(&addr, address))
		return;

	LIF_DICT_FOREACH_SAFE(iter, iter_next, &interface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (strcmp(entry->key, "address"))
			continue;

		struct lif_address *entry_addr = entry->data;
		char addr_buf[512] = {};

		if (!lif_address_unparse(entry_addr, addr_buf, sizeof addr_buf, addr.netmask != 0))
			continue;

		if (strcmp(addr_buf, address))
			continue;

		lif_dict_delete_entry(&interface->vars, entry);
		free(entry_addr);
	}
}

void
lif_interface_fini(struct lif_interface *interface)
{
	struct lif_node *iter, *iter_next;

	LIF_DICT_FOREACH_SAFE(iter, iter_next, &interface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		free(entry->data);
		lif_dict_delete_entry(&interface->vars, entry);
	}

	free(interface->ifname);
}

void
lif_interface_use_executor(struct lif_interface *interface, const char *executor)
{
	char *exec_addon = strdup(executor);

	if (lif_dict_add_once(&interface->vars, "use", exec_addon, (lif_dict_cmp_t) strcmp) == NULL)
		free(exec_addon);

	/* pass requires as compatibility env vars to appropriate executors (bridge, bond) */
	if (!strcmp(executor, "bridge"))
		interface->is_bridge = true;
	else if (!strcmp(executor, "bond"))
		interface->is_bond = true;

	if (strcmp(executor, "dhcp") || !lif_config.use_hostname_for_dhcp)
		return;

	/* learn a reasonable default hostname */
	struct utsname un;
	if (uname(&un) < 0)
		return;

	lif_dict_add(&interface->vars, "hostname", strdup(un.nodename));
}

void
lif_interface_collection_init(struct lif_dict *collection)
{
	struct lif_interface *if_lo;

	memset(collection, '\0', sizeof *collection);

	/* always enable loopback interface as part of a collection */
	if_lo = lif_interface_collection_find(collection, "lo");
	if_lo->is_auto = true;
	if_lo->is_explicit = true;
	lif_interface_use_executor(if_lo, "loopback");
}

void
lif_interface_collection_fini(struct lif_dict *collection)
{
	struct lif_node *iter, *iter_next;

	LIF_DICT_FOREACH_SAFE(iter, iter_next, collection)
	{
		struct lif_dict_entry *entry = iter->data;
		struct lif_interface *iface = entry->data;

		lif_interface_fini(iface);
		free(iface);

		lif_dict_delete_entry(collection, entry);
	}
}

struct lif_interface *
lif_interface_collection_find(struct lif_dict *collection, const char *ifname)
{
	struct lif_dict_entry *entry = lif_dict_find(collection, ifname);

	if (entry == NULL)
	{
		struct lif_interface *iface = calloc(1, sizeof *iface);

		lif_interface_init(iface, ifname);
		lif_dict_add(collection, ifname, iface);

		return iface;
	}

	return entry->data;
}

struct lif_interface *
lif_interface_collection_upsert(struct lif_dict *collection, struct lif_interface *interface)
{
	struct lif_dict_entry *entry = lif_dict_find(collection, interface->ifname);

	if (entry == NULL)
	{
		lif_dict_add(collection, interface->ifname, interface);
		return interface;
	}

	if (entry->data == interface)
		return interface;

	lif_interface_collection_delete(collection, entry->data);
	lif_dict_add(collection, interface->ifname, interface);

	return interface;
}

void
lif_interface_collection_delete(struct lif_dict *collection, struct lif_interface *interface)
{
	struct lif_dict_entry *entry = lif_dict_find(collection, interface->ifname);

	if (entry == NULL)
		return;

	lif_interface_fini(interface);
	free(interface);

	lif_dict_delete_entry(collection, entry);
}

bool
lif_interface_collection_inherit(struct lif_interface *interface, struct lif_interface *parent)
{
	/* maybe convert any interface we are inheriting from into a template */
	if (lif_config.implicit_template_conversion)
		parent->is_template = true;

	lif_dict_add(&interface->vars, "inherit", strdup(parent->ifname));
	interface->is_bond = parent->is_bond;
	interface->is_bridge = parent->is_bridge;

	/* copy the variables */
	struct lif_node *iter;
	LIF_DICT_FOREACH(iter, &parent->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (!strcmp(entry->key, "address"))
		{
			struct lif_address *addr = calloc(1, sizeof *addr);
			struct lif_address *other_addr = entry->data;

			memcpy(addr, other_addr, sizeof *addr);

			lif_dict_add(&interface->vars, entry->key, addr);
		}
		else
		{
			char *value = strdup(entry->data);

			if (lif_dict_add_once(&interface->vars, entry->key, value, (lif_dict_cmp_t) strcmp) == NULL)
				free(value);
		}
	}

	return true;
}
