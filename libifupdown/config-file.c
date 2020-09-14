/*
 * libifupdown/config-file.c
 * Purpose: config file loading
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
#include <stdio.h>
#include <string.h>
#include "libifupdown/libifupdown.h"

struct lif_config_file lif_config = {
	.allow_addon_scripts = true,
};

static bool
set_bool_value(const char *key, const char *value, void *opaque)
{
	(void) key;

	if (*value == '1' ||
		*value == 'Y' || *value == 'y' || 
		*value == 'T' || *value == 't')
		*(bool *) opaque = true;
	else if (*value == '0' ||
		*value == 'N' || *value == 'n' ||
		*value == 'F' || *value == 'f')
		*(bool *) opaque = false;
	else
		return false;

	return true;
}

static struct lif_config_handler handlers[] = {
	{"allow_addon_scripts", set_bool_value, &lif_config.allow_addon_scripts},
};

bool
lif_config_load(const char *filename)
{
	FILE *fd = fopen(filename, "r");

	if (fd == NULL)
	{
#if 0
		fprintf(stderr, "ifupdown-ng: cannot open config %s: %s\n",
			filename, strerror(errno));
#endif
		return false;
	}

	return lif_config_parse_file(fd, filename, handlers, ARRAY_SIZE(handlers));
}
