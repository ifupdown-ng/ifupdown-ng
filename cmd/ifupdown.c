/*
 * cmd/ifupdown.c
 * Purpose: bring interfaces up or down
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
#include <fnmatch.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "libifupdown/libifupdown.h"

struct match_options {
	bool is_auto;
	char *exclude_pattern;
	char *include_pattern;
};

static const char *argv0;
static bool up;
static struct lif_execute_opts exec_opts = {};

void
usage()
{
	fprintf(stderr, "usage: %s [options] <interfaces>\n", argv0);

	fprintf(stderr, "\nOptions:\n");
	fprintf(stderr, "  -h, --help                   this help\n");
	fprintf(stderr, "  -V, --version                show this program's version\n");
	fprintf(stderr, "  -i, --interfaces FILE        use FILE for interface definitions\n");
	fprintf(stderr, "  -S, --state-file FILE        use FILE for state\n");
	fprintf(stderr, "  -a, --auto                   only match against interfaces hinted as 'auto'\n");
	fprintf(stderr, "  -I, --include PATTERN        only match against interfaces matching PATTERN\n");
	fprintf(stderr, "  -X, --exclude PATTERN        never match against interfaces matching PATTERN\n");
	fprintf(stderr, "  -n, --no-act                 do not actually run any commands\n");
	fprintf(stderr, "  -v, --verbose                show what commands are being run\n");

	exit(1);
}

bool
is_ifdown()
{
	if (strstr(argv0, "ifdown") != NULL)
		return true;

	return false;
}

bool
change_interface(struct lif_interface *iface, struct lif_dict *state, const char *ifname)
{
	if (exec_opts.verbose)
	{
		fprintf(stderr, "%s: changing interface %s state to '%s'\n",
			argv0, ifname, up ? "up" : "down");
	}

	if (!lif_lifecycle_run(&exec_opts, iface, state, ifname, up))
	{
		fprintf(stderr, "%s: failed to change interface %s state to '%s'\n",
			argv0, ifname, up ? "up" : "down");
		return false;
	}

	return true;
}

bool
change_auto_interfaces(struct lif_dict *collection, struct lif_dict *state, struct match_options *opts)
{
	struct lif_node *iter;

	LIF_DICT_FOREACH(iter, collection)
	{
		struct lif_dict_entry *entry = iter->data;
		struct lif_interface *iface = entry->data;

		if (opts->is_auto && !iface->is_auto)
			continue;

		if (opts->exclude_pattern != NULL &&
		    !fnmatch(opts->exclude_pattern, iface->ifname, 0))
			continue;

		if (opts->include_pattern != NULL &&
		    fnmatch(opts->include_pattern, iface->ifname, 0))
			continue;

		if (!change_interface(iface, state, iface->ifname))
			return false;
	}

	return true;
}

int
main(int argc, char *argv[])
{
	argv0 = basename(argv[0]);
	up = !is_ifdown();

	struct lif_dict state = {};
	struct lif_dict collection = {};
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'V'},
		{"interfaces", required_argument, 0, 'i'},
		{"auto", no_argument, 0, 'a'},
		{"include", required_argument, 0, 'I'},
		{"exclude", required_argument, 0, 'X'},
		{"state-file", required_argument, 0, 'S'},
		{"no-act", no_argument, 0, 'n'},
		{"verbose", no_argument, 0, 'v'},
		{NULL, 0, 0, 0}
	};
	struct match_options match_opts = {};
	char *interfaces_file = INTERFACES_FILE;
	char *state_file = STATE_FILE;

	for (;;)
	{
		int c = getopt_long(argc, argv, "hVi:aI:X:S:nv", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			usage();
			break;
		case 'V':
			lif_common_version();
			break;
		case 'i':
			interfaces_file = optarg;
			break;
		case 'a':
			match_opts.is_auto = true;
			break;
		case 'I':
			match_opts.include_pattern = optarg;
			break;
		case 'X':
			match_opts.exclude_pattern = optarg;
			break;
		case 'S':
			state_file = optarg;
			break;
		case 'n':
			exec_opts.mock = true;
			exec_opts.verbose = true;
			break;
		case 'v':
			exec_opts.verbose = true;
			break;
		}
	}

	if (!lif_state_read_path(&state, state_file))
	{
		fprintf(stderr, "ifquery: could not parse %s\n", state_file);
		return EXIT_FAILURE;
	}

	if (!lif_interface_file_parse(&collection, interfaces_file))
	{
		fprintf(stderr, "ifquery: could not parse %s\n", interfaces_file);
		return EXIT_FAILURE;
	}

	if (match_opts.is_auto)
	{
		if (!change_auto_interfaces(&collection, &state, &match_opts))
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}
	else if (optind >= argc)
		usage();

	int idx = optind;
	for (; idx < argc; idx++)
	{
		char lifbuf[4096];
		strlcpy(lifbuf, argv[idx], sizeof lifbuf);

		char *ifname = lifbuf;
		char *lifname = lifbuf;
		char *p;

		if ((p = strchr(lifbuf, '=')) != NULL)
		{
			*p++ = '\0';
			lifname = p;
		}

		struct lif_dict_entry *entry = lif_dict_find(&collection, lifname);

		if (entry == NULL)
		{
			fprintf(stderr, "%s: unknown interface %s\n", argv0, argv[idx]);
			return EXIT_FAILURE;
		}

		struct lif_interface *iface = entry->data;
		if (!change_interface(iface, &state, ifname))
			return EXIT_FAILURE;
	}

	if (!exec_opts.mock && !lif_state_write_path(&state, state_file))
	{
		fprintf(stderr, "%s: could not update %s\n", argv0, state_file);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
