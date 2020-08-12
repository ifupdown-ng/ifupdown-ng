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

#define _GNU_SOURCE
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
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

static void
generic_usage(const struct if_applet *applet, int result)
{
	fprintf(stderr, "%s", applet->name);
	if (applet->desc != NULL)
		fprintf(stderr, " - %s", applet->desc);

	fprintf(stderr, "\n");

	size_t iter;
	for (iter = 0; applet->groups[iter] != NULL; iter++)
	{
		const struct if_option_group *group = applet->groups[iter];

		fprintf(stderr, "\n%s:\n", group->desc);

		size_t group_iter;
		for (group_iter = 0; group_iter < group->group_size; group_iter++)
		{
			const struct if_option *opt = &group->group[group_iter];

			fprintf(stderr, "  ");

			if (opt->short_opt)
				fprintf(stderr, "-%c", opt->short_opt);
			else
				fprintf(stderr, "  ");

			if (opt->long_opt)
				fprintf(stderr, "%c --%-30s", opt->short_opt ? ',' : ' ',
					opt->long_opt);
			else
				fprintf(stderr, "%34s", "");

			fprintf(stderr, "%s\n", opt->desc);
		}
	}

	exit(result);
}

static void
generic_usage_request(int short_opt, const struct if_option *option, const char *opt_arg, const struct if_applet *applet)
{
	(void) short_opt;
	(void) option;
	(void) opt_arg;

	generic_usage(applet, EXIT_SUCCESS);
}

static struct if_option global_options[] = {
	{'h', "help", "displays program help", false, generic_usage_request},
	{'V', "version", "displays program version", false, (void *) lif_common_version},
};

struct if_option_group global_option_group = {
	.desc = "Global options",
	.group_size = ARRAY_SIZE(global_options),
	.group = global_options
};

int
applet_cmp(const void *a, const void *b)
{
	const char *key = a;
	const struct if_applet *applet = *(void **)b;

	return strcmp(key, applet->name);
}

void multicall_usage(int status) __attribute__((noreturn));

static const struct if_option *
lookup_option(const struct if_applet *applet, int opt)
{
	size_t iter;
	for (iter = 0; applet->groups[iter] != NULL; iter++)
	{
		const struct if_option_group *group = applet->groups[iter];
		size_t group_iter;

		for (group_iter = 0; group_iter < group->group_size; group_iter++)
		{
			const struct if_option *option = &group->group[group_iter];

			if (option->short_opt == opt)
				return option;
		}
	}

	return NULL;
}

static void
process_options(const struct if_applet *applet, int argc, char *argv[])
{
	char short_opts[256] = {}, *p = short_opts;
	struct option long_opts[256] = {};

	size_t iter, long_opt_iter = 0;
	for (iter = 0; applet->groups[iter] != NULL; iter++)
	{
		const struct if_option_group *group = applet->groups[iter];
		size_t group_iter;

		for (group_iter = 0; group_iter < group->group_size; group_iter++)
		{
			const struct if_option *opt = &group->group[group_iter];

			if (opt->short_opt)
			{
				*p++ = opt->short_opt;
				if (opt->require_argument)
					*p++ = ':';
			}

			if (opt->long_opt)
			{
				/* XXX: handle long-opts without short-opts */
				long_opts[long_opt_iter] = (struct option) {
					.name = opt->long_opt,
					.has_arg = opt->require_argument ? required_argument : no_argument,
					.val = opt->short_opt
				};

				long_opt_iter++;
			}
		}
	}

	for (;;)
	{
		int c = getopt_long(argc, argv, short_opts, long_opts, NULL);
		if (c == -1)
			break;

		const struct if_option *opt = lookup_option(applet, c);
		if (opt == NULL)
			break;

		opt->handle(c, opt, optarg, applet);
	}
}

int
main(int argc, char *argv[])
{
	argv0 = basename(argv[0]);
	const struct if_applet **app;

	app = bsearch(argv0, applet_table,
		      ARRAY_SIZE(applet_table), sizeof(*applet_table),
		      applet_cmp);

	if (app == NULL)
	{
		fprintf(stderr, "%s: applet not found\n", argv0);
		multicall_usage(EXIT_FAILURE);
	}

	process_options(*app, argc, argv);

	return (*app)->main(argc, argv);
}

int
multicall_main(int argc, char *argv[])
{
	if (argc < 2)
		multicall_usage(EXIT_FAILURE);

	return main(argc - 1, argv + 1);
}

struct if_applet ifupdown_applet;

void
multicall_usage(int status)
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

	exit(status);
}

struct if_applet ifupdown_applet = {
	.name = "ifupdown",
	.main = multicall_main,
	.usage = multicall_usage,
	.groups = { &global_option_group, NULL }
};
