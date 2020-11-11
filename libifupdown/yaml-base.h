/*
 * libifupdown/yaml-base.h
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

#ifndef LIBIFUPDOWN_YAML_BASE_H__GUARD
#define LIBIFUPDOWN_YAML_BASE_H__GUARD

#include "libifupdown/libifupdown.h"

/* this is a subset of types supported by our implementation */
enum lif_yaml_value {
	LIF_YAML_STRING,
	LIF_YAML_LIST,
	LIF_YAML_OBJECT,
	LIF_YAML_BOOLEAN
};

struct lif_yaml_node {
	struct lif_node node;

	bool malloced;
	char *name;
	enum lif_yaml_value value_type;
	union {
		char *str_value;			/* for string nodes */
		bool bool_value;			/* for boolean nodes */
	} value;
	struct lif_list children;	/* for list and object nodes */
};

extern void lif_yaml_document_init(struct lif_yaml_node *doc, const char *name);
extern struct lif_yaml_node *lif_yaml_document_new(const char *name);

extern struct lif_yaml_node *lif_yaml_node_new_boolean(const char *name, bool value);
extern struct lif_yaml_node *lif_yaml_node_new_string(const char *name, const char *value);
extern struct lif_yaml_node *lif_yaml_node_new_object(const char *name);
extern struct lif_yaml_node *lif_yaml_node_new_list(const char *name);
extern void lif_yaml_node_free(struct lif_yaml_node *node);
extern void lif_yaml_node_append_child(struct lif_yaml_node *parent, struct lif_yaml_node *child);

#endif
