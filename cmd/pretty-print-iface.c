/*
 * cmd/pretty-print-iface.c
 * Purpose: interface pretty-printer (/e/n/i style)
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
#include <string.h>
#include "libifupdown/libifupdown.h"
#include "cmd/multicall.h"
#include "cmd/pretty-print-iface.h"

void
prettyprint_interface_eni(struct lif_interface *iface)
{
	if (!lif_lifecycle_query_dependents(&exec_opts, iface, iface->ifname))
		return;

	if (iface->is_auto)
		printf("auto %s\n", iface->ifname);

	printf("%s %s", iface->is_template ? "template" : "iface", iface->ifname);

	if (iface->no_defaults)
		printf(" no-defaults");

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

	printf("\n");
}
