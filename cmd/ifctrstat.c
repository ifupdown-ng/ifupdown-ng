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
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "libifupdown/libifupdown.h"
#include "cmd/multicall.h"
#include "cmd/ifctrstat-linux.h"

extern struct counter_desc { const char *name; const void *data; } avail_counters[];
extern int avail_counters_count;

static bool show_label = true;

static bool
counter_is_valid(const char *candidate)
{
	for (int i = 0; i < avail_counters_count; i++)
	{
		if (strcasecmp(avail_counters[i].name, candidate) == 0)
			return true;
	}

	return false;
}

static void
print_counter(const char *iface, const char *name, const char *value)
{
	(void) iface;

	if (show_label)
		fprintf(stdout, "%s: %s\n", name, value);
	else
		fprintf(stdout, "%s\n", value);
}

static int
print_all_counters(const char *iface)
{
	int code = EXIT_SUCCESS;
	const char *res;

	for (int i = 0; i < avail_counters_count; i++)
	{
		const char *ctr = avail_counters[i].name;

		res = read_counter(iface, ctr);
		if (!res)
		{
			fprintf(stderr, "%s: could not determine value of %s for interface %s: %s\n", argv0, ctr, iface, strerror(errno));
			code = EXIT_FAILURE;
		}
		else
		{
			print_counter(iface, ctr, res);
		}
	}

	return code;
}

static void
ifctrstat_list_counters(const char *opt_arg)
{
	(void) opt_arg;

	for (int i = 0; i < avail_counters_count; i++)
	{
		fprintf(stdout, "%s\n", avail_counters[i].name);
	}

	exit(EXIT_SUCCESS);
}

static void
ifctrstat_set_nolabel(const char *opt_arg)
{
	(void) opt_arg;
	show_label = false;
}

static int
ifctrstat_main(int argc, char *argv[])
{
	if (optind >= argc)
		generic_usage(self_applet, EXIT_FAILURE);

	int idx = optind;
	if (argc - idx == 0)
	{
		fprintf(stderr, "%s: interface required\n",
			argv0);
		return EXIT_FAILURE;
	}

	const char *iface = argv[idx++];

	if (argc - idx == 0)
	{
		return print_all_counters(iface);
	}

	for (; idx < argc; idx++)
	{
		if (!counter_is_valid(argv[idx]))
		{
			fprintf(stderr, "%s: counter %s is not valid or not available\n", argv0, argv[idx]);
			return EXIT_FAILURE;
		}

		errno = 0;
		const char *res = read_counter(iface, argv[idx]);
		if (!res)
		{
			fprintf(stderr, "%s: could not determine value of %s for interface %s: %s\n", argv0, argv[idx], iface, strerror(errno));
			return EXIT_FAILURE;
		}

		print_counter(iface, argv[idx], res);
	}

	return EXIT_SUCCESS;
}

static struct if_option local_options[] = {
	{'L', "list", NULL, "list available counters", false, ifctrstat_list_counters},
	{'n', "no-label", NULL, "print value without counter label", false, ifctrstat_set_nolabel}
};

static struct if_option_group local_option_group = {
	.desc = "Program-specific options",
	.group_size = ARRAY_SIZE(local_options),
	.group = local_options
};

struct if_applet ifctrstat_applet = {
	.name = "ifctrstat",
	.desc = "display statistics about an interface",
	.main = ifctrstat_main,
	.usage = "ifctrstat [options] <interface> <counter>\n  ifctrstat [options] --list",
	.manpage = "8 ifctrstat",
	.groups = { &global_option_group, &local_option_group, NULL }
};
