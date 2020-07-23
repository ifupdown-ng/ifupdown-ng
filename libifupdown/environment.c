/*
 * libifupdown/environment.c
 * Purpose: environment variable management
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
#include <stdlib.h>
#include <string.h>
#include "libifupdown/environment.h"

bool
lif_environment_push(char **env[], const char *name, const char *val)
{
	char buf[4096];

	snprintf(buf, sizeof buf, "%s=%s", name, val);

	/* create an initial envp: {"foo=bar", NULL} */
	if (*env == NULL)
	{
		*env = calloc(2, sizeof (char *));
		(*env)[0] = strdup(buf);
		(*env)[1] = NULL;

		return true;
	}

	size_t nelems;
	for (nelems = 0; (*env)[nelems] != NULL; nelems++)
		;

	/* NULL at end, plus next env var */
	size_t allocelems = nelems + 2;
	*env = realloc(*env, ((allocelems + 2) * sizeof (char *)));

	(*env)[nelems] = strdup(buf);
	(*env)[nelems + 1] = NULL;

	return true;
}

void
lif_environment_free(char **env[])
{
	size_t nelems;

	for (nelems = 0; (*env)[nelems] != NULL; nelems++)
		free((*env)[nelems]);

	free(*env);
}
