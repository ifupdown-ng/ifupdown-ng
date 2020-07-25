/*
 * cmd/multicall.c
 * Purpose: multi-call binary frontend
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

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd/multicall.h"

char *argv0;

extern struct if_applet ifquery_applet;
extern struct if_applet ifup_applet;
extern struct if_applet ifdown_applet;
struct if_applet ifupdown_applet;

struct if_applet *applet_table[] = {
	&ifdown_applet,
	&ifquery_applet,
	&ifup_applet,
	&ifupdown_applet,
};

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(*x))

int
applet_cmp(const void *a, const void *b)
{
	const char *key = a;
	const struct if_applet *applet = *(void **)b;

	return strcmp(key, applet->name);
}

void multicall_usage(void) __attribute__((noreturn));

int
main(int argc, char *argv[])
{
	argv0 = basename(argv[0]);
	struct if_applet **app;

	app = bsearch(argv0, applet_table,
		      ARRAY_SIZE(applet_table), sizeof(*applet_table),
		      applet_cmp);

	if (app == NULL)
	{
		fprintf(stderr, "%s: applet not found\n", argv0);
		multicall_usage();
	}

	return (*app)->main(argc, argv);
}

int
multicall_main(int argc, char *argv[])
{
	if (argc < 2)
		multicall_usage();

	return main(argc - 1, argv + 1);
}

void
multicall_usage(void)
{
	fprintf(stderr, "usage: ifupdown <applet> [options]\n");

	fprintf(stderr, "\nBuilt-in applets:\n\t");
	for (size_t i = 0; i < ARRAY_SIZE(applet_table); i++)
	{
		if (i != 0)
			fprintf(stderr, ", ");

		fprintf(stderr, "%s", applet_table[i]->name);
	}

	fprintf(stderr, "\n");

	exit(EXIT_FAILURE);
}

struct if_applet ifupdown_applet = {
	.name = "ifupdown",
	.main = multicall_main,
	.usage = multicall_usage,
};
