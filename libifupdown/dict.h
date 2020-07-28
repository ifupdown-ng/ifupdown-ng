/*
 * libifupdown/dict.h
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

#ifndef LIBIFUPDOWN_DICT_H__GUARD
#define LIBIFUPDOWN_DICT_H__GUARD

#include "libifupdown/list.h"

struct lif_dict {
	struct lif_list list;
};

struct lif_dict_entry {
	struct lif_node node;
	char *key;
	void *data;
};

#define LIF_DICT_FOREACH(iter, dict) \
	LIF_LIST_FOREACH((iter), (dict)->list.head)

#define LIF_DICT_FOREACH_SAFE(iter, iter_next, dict) \
	LIF_LIST_FOREACH_SAFE((iter), (iter_next), (dict)->list.head)

extern void lif_dict_init(struct lif_dict *dict);
extern void lif_dict_fini(struct lif_dict *dict);
extern struct lif_dict_entry *lif_dict_add(struct lif_dict *dict, const char *key, void *data);
extern struct lif_dict_entry *lif_dict_add_once(struct lif_dict *dict, const char *key, void *data, int (*compar)(const void *, const void *));
extern struct lif_dict_entry *lif_dict_find(struct lif_dict *dict, const char *key);
extern struct lif_list *lif_dict_find_all(struct lif_dict *dict, const char *key);
extern void lif_dict_delete(struct lif_dict *dict, const char *key);
extern void lif_dict_delete_entry(struct lif_dict *dict, struct lif_dict_entry *entry);

#endif
