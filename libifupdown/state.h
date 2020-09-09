/*
 * libifupdown/state.h
 * Purpose: state management
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

#ifndef LIBIFUPDOWN_STATE_H__GUARD
#define LIBIFUPDOWN_STATE_H__GUARD

#include <stdio.h>
#include "libifupdown/interface.h"

struct lif_state_record {
	char *mapped_if;
	size_t refcount;
};

extern bool lif_state_read(struct lif_dict *state, FILE *f);
extern bool lif_state_read_path(struct lif_dict *state, const char *path);
extern void lif_state_upsert(struct lif_dict *state, const char *ifname, struct lif_interface *iface);
extern void lif_state_ref_if(struct lif_dict *state, const char *ifname, struct lif_interface *iface);
extern void lif_state_unref_if(struct lif_dict *state, const char *ifname, struct lif_interface *iface);
extern void lif_state_delete(struct lif_dict *state, const char *ifname);
extern void lif_state_write(const struct lif_dict *state, FILE *f);
extern bool lif_state_write_path(const struct lif_dict *state, const char *path);
extern struct lif_interface *lif_state_lookup(struct lif_dict *state, struct lif_dict *if_collection, const char *ifname);
extern bool lif_state_sync(struct lif_dict *state, struct lif_dict *if_collection);

#endif
