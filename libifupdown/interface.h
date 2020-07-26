/*
 * libifupdown/interface.h
 * Purpose: interface management
 *
 * Copyright (c) 2020 Ariadne Conill <ariadne@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#ifndef LIBIFUPDOWN_INTERFACE_H__GUARD
#define LIBIFUPDOWN_INTERFACE_H__GUARD

#include <arpa/inet.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "libifupdown/dict.h"

/*
 * Addresses are stored in a buffer with netmask and type.
 */
struct lif_address {
	unsigned char addr_buf[sizeof(struct in6_addr)];
	size_t netmask;
	int domain;
};

extern bool lif_address_parse(struct lif_address *address, const char *presentation);
extern bool lif_address_unparse(const struct lif_address *address, char *buf, size_t buflen, bool with_netmask);

/*
 * Interfaces are contained in a dictionary, with the interfaces mapped by
 * interface name to their `struct lif_interface`.
 *
 * Interfaces are simply upserted as needed.  This allows for `auto eth0`
 * to create a placeholder `struct lif_interface` with auto set to true.
 *
 * Configuration variables are simply stored in a `struct lif_dict`, which
 * can act as a multidict.
 */
struct lif_interface {
	char *ifname;

	bool is_dhcp;
	bool is_loopback;
	bool is_auto;
	bool is_bridge;
	bool is_bond;
	bool is_static;

	struct lif_dict vars;

	bool is_up;
};

#define LIF_INTERFACE_COLLECTION_FOREACH(iter, collection) \
	LIF_DICT_FOREACH((iter), (collection))

#define LIF_INTERFACE_COLLECTION_FOREACH_SAFE(iter, iter_next, collection) \
	LIF_DICT_FOREACH_SAFE((iter), (iter_next), (collection))

extern void lif_interface_init(struct lif_interface *interface, const char *ifname);
extern bool lif_interface_address_add(struct lif_interface *interface, const char *address);
extern void lif_interface_address_delete(struct lif_interface *interface, const char *address);
extern void lif_interface_fini(struct lif_interface *interface);

extern void lif_interface_collection_init(struct lif_dict *collection);
extern void lif_interface_collection_fini(struct lif_dict *collection);
extern struct lif_interface *lif_interface_collection_find(struct lif_dict *collection, const char *ifname);
extern struct lif_interface *lif_interface_collection_upsert(struct lif_dict *collection, struct lif_interface *interface);
extern void lif_interface_collection_delete(struct lif_dict *collection, struct lif_interface *interface);

#endif
