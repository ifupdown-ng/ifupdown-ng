/*
 * libifupdown/list.c
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

#include <stdint.h>
#include <stdlib.h>
#include "libifupdown/list.h"

void
lif_list_free_nodes(struct lif_list *list)
{
	if (list == NULL)
		return;

	struct lif_node *iter, *iter_next;

	LIF_LIST_FOREACH_SAFE(iter, iter_next, list->head)
	{
		free (iter);
	}

	free (list);
}

void
lif_node_insert(struct lif_node *node, void *data, struct lif_list *list)
{
	struct lif_node *tnode;

	node->data = data;

	if (list->head == NULL)
	{
		list->head = list->tail = node;
		list->length = 1;
		return;
	}

	tnode = list->head;

	node->next = tnode;
	tnode->prev = node;

	list->head = node;
	list->length++;
}

void
lif_node_insert_tail(struct lif_node *node, void *data, struct lif_list *list)
{
	struct lif_node *tnode;

	node->data = data;

	if (list->tail == NULL)
	{
		list->head = list->tail = node;
		list->length = 1;
		return;
	}

	tnode = list->tail;

	node->prev = tnode;
	tnode->next = node;

	list->tail = node;
	list->length++;
}

void
lif_node_delete(struct lif_node *node, struct lif_list *list)
{
	list->length--;

	if (node->prev == NULL)
		list->head = node->next;
	else
		node->prev->next = node->next;

	if (node->next == NULL)
		list->tail = node->prev;
	else
		node->next->prev = node->prev;
}
