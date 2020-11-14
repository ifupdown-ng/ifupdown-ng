/*
 * libifupdown/yaml-base.c
 * Purpose: YAML implementation -- base
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
#include "libifupdown/libifupdown.h"
#include "libifupdown/yaml-base.h"

void
lif_yaml_document_init(struct lif_yaml_node *doc, const char *name)
{
	memset(doc, '\0', sizeof *doc);
	doc->value_type = LIF_YAML_OBJECT;

	if (name != NULL)
		doc->name = strdup(name);
}

struct lif_yaml_node *
lif_yaml_document_new(const char *name)
{
	struct lif_yaml_node *doc = calloc(1, sizeof *doc);

	lif_yaml_document_init(doc, name);
	doc->malloced = true;

	return doc;
}

struct lif_yaml_node *
lif_yaml_node_new_boolean(const char *name, bool value)
{
	struct lif_yaml_node *node = calloc(1, sizeof *node);

	node->malloced = true;
	node->value_type = LIF_YAML_BOOLEAN;

	if (name != NULL)
		node->name = strdup(name);

	node->value.bool_value = value;

	return node;
}

struct lif_yaml_node *
lif_yaml_node_new_string(const char *name, const char *value)
{
	struct lif_yaml_node *node = calloc(1, sizeof *node);

	node->malloced = true;
	node->value_type = LIF_YAML_STRING;

	if (name != NULL)
		node->name = strdup(name);

	if (value != NULL)
		node->value.str_value = strdup(value);

	return node;
}

struct lif_yaml_node *
lif_yaml_node_new_object(const char *name)
{
	struct lif_yaml_node *node = calloc(1, sizeof *node);

	node->malloced = true;
	node->value_type = LIF_YAML_OBJECT;

	if (name != NULL)
		node->name = strdup(name);

	return node;
}

struct lif_yaml_node *
lif_yaml_node_new_list(const char *name)
{
	struct lif_yaml_node *node = calloc(1, sizeof *node);

	node->malloced = true;
	node->value_type = LIF_YAML_LIST;

	if (name != NULL)
		node->name = strdup(name);

	return node;
}

void
lif_yaml_node_free(struct lif_yaml_node *node)
{
	struct lif_node *iter, *next;

	LIF_LIST_FOREACH_SAFE(iter, next, node->children.head)
	{
		struct lif_yaml_node *iter_node = iter->data;

		lif_yaml_node_free(iter_node);
	}

	free(node->name);

	if (node->value_type == LIF_YAML_STRING)
		free(node->value.str_value);

	if (node->malloced)
		free(node);
}

void
lif_yaml_node_append_child(struct lif_yaml_node *parent, struct lif_yaml_node *child)
{
	lif_node_insert_tail(&child->node, child, &parent->children);
}
