/*
 * libifupdown/config-parser.h
 * Purpose: config parsing
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

#ifndef LIBIFUPDOWN__CONFIG_PARSER_H
#define LIBIFUPDOWN__CONFIG_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct lif_config_handler {
	const char *key;
	bool (*handle)(const char *key, const char *value, void *opaque);
	void *opaque;
};

extern bool lif_config_parse_file(FILE *f, const char *filename, struct lif_config_handler *handlers, size_t handler_count);
extern bool lif_config_parse(const char *filename, struct lif_config_handler *handlers, size_t handler_count);

#endif
