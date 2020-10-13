/*
 * libifupdown/dict.c
 * Purpose: wrapping linked lists to provide a naive dictionary
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

#include <stdbool.h>
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

	lif_node_insert_tail(&entry->node, entry, &dict->list);

	return entry;
}

struct lif_dict_entry *
lif_dict_add_once(struct lif_dict *dict, const char *key, void *data,
                  lif_dict_cmp_t compar)
{
	struct lif_list *existing = lif_dict_find_all(dict, key);
	if (existing != NULL)
	{
		bool found = false;
		struct lif_node *iter;
		LIF_LIST_FOREACH(iter, existing->head)
		{
			if (!compar(data, iter->data))
			{
				found = true;
				break;
			}
		}

		lif_list_free_nodes(existing);

		if (found)
			return NULL;
	}

	struct lif_dict_entry *entry = calloc(1, sizeof *entry);

	entry->key = strdup(key);
	entry->data = data;

	lif_node_insert_tail(&entry->node, entry, &dict->list);

	return entry;
}

struct lif_dict_entry *
lif_dict_find(const struct lif_dict *dict, const char *key)
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

struct lif_list *
lif_dict_find_all(const struct lif_dict *dict, const char *key)
{
	struct lif_list *entries = calloc(1, sizeof *entries);
	struct lif_node *iter;

	LIF_LIST_FOREACH(iter, dict->list.head)
	{
		struct lif_dict_entry *entry = iter->data;
		if (!strcmp(entry->key, key))
		{
			struct lif_node *new = calloc(1, sizeof *new);
			lif_node_insert_tail(new, entry->data, entries);
		}
	}

	if (entries->length == 0)
	{
		free(entries);
		return NULL;
	}

	return entries;
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
