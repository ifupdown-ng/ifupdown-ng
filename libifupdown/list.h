/*
 * libifupdown/list.h
 * Purpose: linked lists
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

#ifndef LIBIFUPDOWN_LIST_H__GUARD
#define LIBIFUPDOWN_LIST_H__GUARD

#include <stddef.h>
#include <stdint.h>

struct lif_node {
	struct lif_node *prev, *next;
	void *data;
};

struct lif_list {
	struct lif_node *head, *tail;
	size_t length;
};

extern void lif_list_free_nodes(struct lif_list *list);

extern void lif_node_insert(struct lif_node *node, void *data, struct lif_list *list);
extern void lif_node_insert_tail(struct lif_node *node, void *data, struct lif_list *list);
extern void lif_node_delete(struct lif_node *node, struct lif_list *list);

#define LIF_LIST_FOREACH(iter, head) \
	for ((iter) = (head); (iter) != NULL; (iter) = (iter)->next)

#define LIF_LIST_FOREACH_SAFE(iter, iter_next, head) \
	for ((iter) = (head), (iter_next) = (iter)->next; (iter) != NULL; (iter) = (iter_next), (iter_next) = (iter) != NULL ? (iter)->next : NULL)

#endif
