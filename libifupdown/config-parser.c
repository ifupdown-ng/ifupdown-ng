/*
 * libifupdown/config-parser.c
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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libifupdown/config-parser.h"
#include "libifupdown/fgetline.h"
#include "libifupdown/tokenize.h"

static int
handler_cmp(const void *a, const void *b)
{
	const char *key = a;
	const struct lif_config_handler *hdl = b;

	return strcmp(key, hdl->key);
}

bool
lif_config_parse_file(FILE *fd, const char *filename, struct lif_config_handler *handlers, size_t handler_count)
{
	char linebuf[4096];
	size_t lineno = 0;

	while (lif_fgetline(linebuf, sizeof linebuf, fd))
	{
		char *bufp = linebuf;
		char *key = lif_next_token_eq(&bufp);
		char *value = lif_next_token_eq(&bufp);

		lineno++;

		if (!*key || !*value)
			continue;

		if (*key == '#')
			continue;

		struct lif_config_handler *hdl = bsearch(key, handlers, handler_count, sizeof(*handlers),
			handler_cmp);

		if (hdl == NULL)
		{
			fprintf(stderr, "ifupdown-ng: %s:%zu: warning: unknown config setting %s\n",
				filename, lineno, key);
			continue;
		}

		if (!hdl->handle(key, value, hdl->opaque))
		{
			fclose(fd);
			return false;
		}
	}

	fclose(fd);
	return true;
}

bool
lif_config_parse(const char *filename, struct lif_config_handler *handlers, size_t handler_count)
{
	FILE *f = fopen(filename, "r");

	if (f == NULL)
	{
		fprintf(stderr, "ifupdown-ng: unable to parse %s: %s\n", filename, strerror(errno));
		return false;
	}

	return lif_config_parse_file(f, filename, handlers, handler_count);
}
