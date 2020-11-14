/*
 * libifupdown/yaml-writer.h
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

#ifndef LIBIFUPDOWN_YAML_WRITER_H__GUARD
#define LIBIFUPDOWN_YAML_WRITER_H__GUARD

#include "libifupdown/libifupdown.h"
#include "libifupdown/yaml-base.h"

extern void lif_yaml_write(struct lif_yaml_node *doc, FILE *f, bool type_annotations);

#endif
