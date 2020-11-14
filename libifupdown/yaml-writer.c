/*
 * libifupdown/yaml-writer.c
 * Purpose: YAML implementation -- writer
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

#include <stdio.h>
#include <string.h>
#include "libifupdown/libifupdown.h"
#include "libifupdown/yaml-base.h"
#include "libifupdown/yaml-writer.h"

static const size_t INDENT_WIDTH = 2;

static void
lif_yaml_write_node(struct lif_yaml_node *node, FILE *f, size_t indent, bool type_annotations)
{
	struct lif_node *iter;

	if (node->name != NULL)
		fprintf(f, "%*s%s: ", (int) indent, "", node->name);

	size_t child_indent = indent + INDENT_WIDTH;

	switch (node->value_type)
	{
	case LIF_YAML_BOOLEAN:
		fprintf(f, "%s%s\n", type_annotations ? "!!bool " : "", node->value.bool_value ? "true" : "false");
		break;
	case LIF_YAML_STRING:
		fprintf(f, "%s%s\n", type_annotations ? "!!str " : "", node->value.str_value);
		break;
	case LIF_YAML_OBJECT:
		fprintf(f, "\n");
		break;
	case LIF_YAML_LIST:
		fprintf(f, "\n");
		child_indent += INDENT_WIDTH;
		break;
	}

	LIF_LIST_FOREACH(iter, node->children.head)
	{
		struct lif_yaml_node *iter_node = iter->data;

		if (node->value_type == LIF_YAML_LIST)
			fprintf(f, "%*s-\n", (int) (child_indent - INDENT_WIDTH), "");

		lif_yaml_write_node(iter_node, f, child_indent, type_annotations);
	}
}

void
lif_yaml_write(struct lif_yaml_node *doc, FILE *f, bool type_annotations)
{
	lif_yaml_write_node(doc, f, 0, type_annotations);
}
