/*
 * libifupdown/state.c
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

#include <string.h>
#include "libifupdown/state.h"
#include "libifupdown/fgetline.h"

bool
lif_state_read(struct lif_dict *state, FILE *fd)
{
	char linebuf[4096];
	while (lif_fgetline(linebuf, sizeof linebuf, fd))
	{
		char *ifname = linebuf;
		char *equals_p = strchr(linebuf, '=');

		if (equals_p == NULL)
		{
			lif_state_upsert(state, ifname, &(struct lif_interface){ .ifname = ifname });
			continue;
		}

		*equals_p++ = '\0';
		lif_state_upsert(state, ifname, &(struct lif_interface){ .ifname = equals_p });
	}

	return true;
}

bool
lif_state_read_path(struct lif_dict *state, const char *path)
{
	FILE *fd = fopen(path, "r");
	bool ret;

	/* if file cannot be opened, assume an empty state */
	if (fd == NULL)
		return true;

	ret = lif_state_read(state, fd);
	fclose(fd);

	return ret;
}

void
lif_state_upsert(struct lif_dict *state, const char *ifname, struct lif_interface *iface)
{
	lif_dict_add(state, ifname, strdup(iface->ifname));
}

void
lif_state_delete(struct lif_dict *state, const char *ifname)
{
	struct lif_dict_entry *entry = lif_dict_find(state, ifname);

	if (entry == NULL)
		return;

	free(entry->data);
	lif_dict_delete_entry(state, entry);
}

void
lif_state_write(const struct lif_dict *state, FILE *f)
{
	struct lif_node *iter;

	LIF_DICT_FOREACH(iter, state)
	{
		struct lif_dict_entry *entry = iter->data;

		fprintf(f, "%s=%s\n", entry->key, (const char *) entry->data);
	}
}

bool
lif_state_write_path(const struct lif_dict *state, const char *path)
{
	FILE *fd = fopen(path, "w");

	if (fd == NULL)
		return false;

	lif_state_write(state, fd);
	fclose(fd);

	return true;
}

struct lif_interface *
lif_state_lookup(struct lif_dict *state, struct lif_dict *if_collection, const char *ifname)
{
	struct lif_dict_entry *entry = lif_dict_find(state, ifname);

	if (entry == NULL)
		return NULL;

	struct lif_dict_entry *if_entry = lif_dict_find(if_collection, (const char *) entry->data);

	if (if_entry == NULL)
		return NULL;

	return if_entry->data;
}

bool
lif_state_sync(struct lif_dict *state, struct lif_dict *if_collection)
{
	struct lif_node *iter;

	LIF_DICT_FOREACH(iter, state)
	{
		struct lif_dict_entry *entry = iter->data;
		struct lif_interface *iface = lif_interface_collection_find(if_collection, entry->key);

		iface->is_up = true;
	}

	return true;
}
