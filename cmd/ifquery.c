/*
 * cmd/ifquery.c
 * Purpose: look up information in /etc/network/interfaces
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

void
print_interface(struct lif_interface *iface)
{
	if (iface->is_auto)
		printf("auto %s\n", iface->ifname);

	printf("iface %s", iface->ifname);

	if (iface->is_loopback)
		printf(" inet loopback\n");
	else if (iface->is_dhcp)
		printf(" inet dhcp\n");
	else
		printf("\n");

	struct lif_node *iter;
	LIF_DICT_FOREACH(iter, &iface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (!strcmp(entry->key, "address"))
		{
			struct lif_address *addr = entry->data;
			char addr_buf[512];

			if (!lif_address_unparse(addr, addr_buf, sizeof addr_buf, true))
			{
				printf("  # warning: failed to unparse address\n");
				continue;
			}

			printf("  %s %s\n", entry->key, addr_buf);
		}
		else
			printf("  %s %s\n", entry->key, (const char *) entry->data);
	}
}

void
usage()
{
	printf("usage: ifquery [options] <interfaces>\n");
	printf("       ifquery [options] --list\n");

	printf("\nOptions:\n");
	printf("  -h, --help                   this help\n");
	printf("  -V, --version                show this program's version\n");
	printf("  -i, --interfaces FILE        use FILE for interface definitions\n");
	printf("  -L, --list                   list matching interfaces\n");
	printf("  -a, --auto                   only match against interfaces hinted as 'auto'\n");
	printf("  -I, --include PATTERN        only match against interfaces matching PATTERN\n");
	printf("  -X, --exclude PATTERN        never match against interfaces matching PATTERN\n");

	exit(1);
}

struct match_options {
	bool is_auto;
	char *exclude_pattern;
	char *include_pattern;
};

void
list_interfaces(struct lif_dict *collection, struct match_options *opts)
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

		printf("%s\n", iface->ifname);
	}
}

int
main(int argc, char *argv[])
{
	struct lif_dict collection;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'V'},
		{"interfaces", required_argument, 0, 'i'},
		{"list", no_argument, 0, 'L'},
		{"auto", no_argument, 0, 'a'},
		{"include", required_argument, 0, 'I'},
		{"exclude", required_argument, 0, 'X'},
		{NULL, 0, 0, 0}
	};
	struct match_options match_opts = {};
	bool listing = false;
	char *interfaces_file = INTERFACES_FILE;

	for (;;)
	{
		int c = getopt_long(argc, argv, "hVi:LaI:X:", long_options, NULL);
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
		case 'L':
			listing = true;
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
		}
	}

	if (!lif_interface_file_parse(&collection, interfaces_file))
	{
		fprintf(stderr, "ifquery: could not parse %s\n", interfaces_file);
		return EXIT_FAILURE;
	}

	if (listing)
	{
		list_interfaces(&collection, &match_opts);
		return EXIT_SUCCESS;
	}

	if (optind >= argc)
		usage();

	int idx = optind;
	for (; idx < argc; idx++)
	{
		struct lif_dict_entry *entry = lif_dict_find(&collection, argv[idx]);
		if (entry == NULL)
		{
			fprintf(stderr, "ifquery: unknown interface %s\n", argv[idx]);
			return EXIT_FAILURE;
		}

		print_interface(entry->data);
	}

	return EXIT_SUCCESS;
}
