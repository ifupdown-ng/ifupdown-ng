/*
 * libifupdown/dict.c
 * Purpose: wrapping linked lists to provide a naive dictionary
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

#include <stdlib.h>
#include <string.h>
#include "libifupdown/dict.h"

void
lif_dict_init(struct lif_dict *dict)
{
	memset(dict, 0, sizeof *dict);
}

void
lif_dict_fini(struct lif_dict *dict)
{
	struct lif_node *iter, *iter_next;

	LIF_DICT_FOREACH_SAFE(iter, iter_next, dict)
	{
		struct lif_dict_entry *entry = iter->data;

		lif_dict_delete_entry(dict, entry);
	}
}

struct lif_dict_entry *
lif_dict_add(struct lif_dict *dict, const char *key, void *data)
{
	struct lif_dict_entry *entry = calloc(1, sizeof *entry);

	entry->key = strdup(key);
	entry->data = data;

	lif_node_insert(&entry->node, entry, &dict->list);

	return entry;
}

struct lif_dict_entry *
lif_dict_find(struct lif_dict *dict, const char *key)
{
	struct lif_node *iter;

	LIF_DICT_FOREACH(iter, dict)
	{
		struct lif_dict_entry *entry = iter->data;

		if (!strcmp(entry->key, key))
			return entry;
	}

	return NULL;
}

void
lif_dict_delete(struct lif_dict *dict, const char *key)
{
	struct lif_dict_entry *entry = lif_dict_find(dict, key);

	if (entry == NULL)
		return;

	lif_dict_delete_entry(dict, entry);
}

void
lif_dict_delete_entry(struct lif_dict *dict, struct lif_dict_entry *entry)
{
	lif_node_delete(&entry->node, &dict->list);

	free(entry->key);
	free(entry);
}
