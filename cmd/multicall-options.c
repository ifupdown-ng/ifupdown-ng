/*
 * cmd/multicall-options.c
 * Purpose: multi-call binary frontend -- option handling
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

extern const struct if_applet *self_applet;

void
generic_usage(const struct if_applet *applet, int result)
{
	fprintf(stderr, "%s", applet->name);
	if (applet->desc != NULL)
		fprintf(stderr, " - %s", applet->desc);

	fprintf(stderr, "\n");

	if (applet->usage != NULL)
		fprintf(stderr, "\nUsage:\n  %s\n", applet->usage);

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
					opt->long_opt_desc ? opt->long_opt_desc : opt->long_opt);
			else
				fprintf(stderr, "%34s", "");

			fprintf(stderr, "%s\n", opt->desc);
		}
	}

	if (applet->manpage != NULL)
		fprintf(stderr, "\nFor more information: man %s\n", applet->manpage);

	exit(result);
}

static void
generic_usage_request(const char *opt_arg)
{
	(void) opt_arg;

	generic_usage(self_applet, EXIT_SUCCESS);
}

static struct if_option global_options[] = {
	{'h', "help", NULL, "this help", false, generic_usage_request},
	{'V', "version", NULL, "show this program's version", false, (void *) lif_common_version},
};

struct if_option_group global_option_group = {
	.desc = "Global options",
	.group_size = ARRAY_SIZE(global_options),
	.group = global_options
};

const struct if_option *
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

void
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

		opt->handle(optarg);
	}
}
