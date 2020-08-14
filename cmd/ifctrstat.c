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

extern const char *avail_counters[];
extern int avail_counters_count;

extern const char *read_counter(const char *interface, const char *counter);

static bool show_label = true;

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

static void
print_counter(const char *iface, const char *name, const char *value)
{
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

	for (int i = 0; i < avail_counters_count; i++) {
		const char *ctr = avail_counters[i];

		res = read_counter(iface, ctr);
		if (!res)
		{
			fprintf(stderr, "%s: could not determine value of %s for interface %s: %s\n", argv0, ctr, iface, strerror(errno));
			code = EXIT_FAILURE;
		} else {
			print_counter(iface, ctr, res);
		}
	}

	return code;
}

void
ifctrstat_list_counters(int short_opt, const struct if_option *opt, const char *opt_arg, const struct if_applet *applet)
{
	(void) short_opt;
	(void) opt;
	(void) opt_arg;
	(void) applet;

	for (int i = 0; i < avail_counters_count; i++)
	{
		fprintf(stdout, "%s\n", avail_counters[i]);
	}

	exit(EXIT_SUCCESS);
}

void
ifctrstat_set_nolabel(int short_opt, const struct if_option *opt, const char *opt_arg, const struct if_applet *applet)
{
	(void) short_opt;
	(void) opt;
	(void) opt_arg;
	(void) applet;

	show_label = false;
}

int
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
	{'L', "list", NULL, "List available counters", false, ifctrstat_list_counters},
	{'n', "no-label", NULL, "Print value without counter label", false, ifctrstat_set_nolabel}
};

static struct if_option_group local_option_group = {
	.desc = "Program-specific options",
	.group_size = ARRAY_SIZE(local_options),
	.group = local_options
};

struct if_applet ifctrstat_applet = {
	.name = "ifctrstat",
	.desc = "Display statistics about an interface",
	.main = ifctrstat_main,
	.usage = "ifctrstat [options] <interface> <counter>\n  ifctrstat [options] --list\n",
	.groups = { &global_option_group, &local_option_group, NULL }
};
