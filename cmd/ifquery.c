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
	printf("  -i, --interfaces FILE        use FILE for interface definitions\n");

	exit(1);
}

int
main(int argc, char *argv[])
{
	struct lif_dict collection;
	struct option long_options[] = {
		{"interfaces", required_argument, 0, 'i'},
		{NULL, 0, 0, 0}
	};
	char *interfaces_file = INTERFACES_FILE;

	for (;;)
	{
		int c = getopt_long(argc, argv, "i:", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'i':
			interfaces_file = optarg;
			break;
		}
	}

	lif_interface_collection_init(&collection);

	if (optind >= argc)
		usage();

	int idx = optind;
	for (; idx < argc; idx++)
	{
		struct lif_dict_entry *entry = lif_dict_find(&collection, argv[idx]);
		if (entry == NULL)
		{
			printf("ifquery: unknown interface %s\n", argv[idx]);
			return EXIT_FAILURE;
		}

		print_interface(entry->data);
	}

	return EXIT_SUCCESS;
}
