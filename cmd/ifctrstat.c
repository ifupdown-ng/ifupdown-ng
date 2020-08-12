/*
 * cmd/ifctrstat.c
 * Purpose: Display statistics about interfaces on the system
 *
 * Copyright (c) 2020 Adélie Software in the Public Benefit, Inc.
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
#include <getopt.h>
#include "libifupdown/libifupdown.h"
#include "cmd/multicall.h"

extern const char *avail_counters[];
extern int avail_counters_count;

extern const char *read_counter(const char *interface, const char *counter);

static bool
counter_is_valid(const char *candidate)
{
	for (int i = 0; i < avail_counters_count; i++)
	{
		if (strcasecmp(avail_counters[i], candidate) == 0)
			return true;
	}

	return false;
}

void
ifstats_list_counters(int short_opt, const struct if_option *opt,
		const char *opt_arg, const struct if_applet *applet)
{
	(void) short_opt;
	(void) opt;
	(void) opt_arg;
	(void) applet;

	for (int i = 0; i < avail_counters_count; i++) {
		fprintf(stdout, "%s\n", avail_counters[i]);
	}

	exit(EXIT_SUCCESS);
}

int
ifstats_main(int argc, char *argv[])
{
	if (optind >= argc)
		generic_usage(self_applet, EXIT_FAILURE);

	int idx = optind;
	if (argc - idx < 2)
	{
		fprintf(stderr, "%s: interface and counter(s) required\n",
			argv0);
		return EXIT_FAILURE;
	}

	const char *iface = argv[idx++];

	for (; idx < argc; idx++)
	{
		if (!counter_is_valid(argv[idx]))
		{
			fprintf(stderr, "%s: counter %s is not valid or not "
					"available\n", argv0, argv[idx]);
			return EXIT_FAILURE;
		}

		errno = 0;
		const char *res = read_counter(iface, argv[idx]);
		if (!res)
		{
			fprintf(stderr, "%s: could not determine value of "
					"%s for interface %s: %s\n", argv0,
					argv[idx], iface, strerror(errno));
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

static struct if_option local_options[] = {
	{'L', "list", NULL, "List available counters", false, ifstats_list_counters}
};

static struct if_option_group local_option_group = {
	.desc = "Program-specific options",
	.group_size = ARRAY_SIZE(local_options),
	.group = local_options
};

struct if_applet ifctrstat_applet = {
	.name = "ifctrstat",
	.desc = "Display statistics about an interface",
	.main = ifstats_main,
	.usage = "ifctrstat [options] <interface> <counter>\n  ifctrstat [options] --list\n",
	.groups = { &global_option_group, &local_option_group, NULL }
};
