/*
 * libifupdown/compat.c
 * Purpose: compatiblity glue to other implementations
 *
 * Copyright (c) 2020 Maximilian Wilhelm <max@sdn.clinic>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "libifupdown/compat.h"
#include "libifupdown/config-file.h"
#include "libifupdown/dict.h"
#include "libifupdown/interface.h"
#include "libifupdown/tokenize.h"

static bool
compat_ifupdown2_bridge_ports_inherit_vlans(struct lif_dict *collection)
{
	struct lif_node *iter;

	/* Loop through all interfaces and search for bridges */
	LIF_DICT_FOREACH(iter, collection)
	{
		struct lif_dict_entry *entry = iter->data;
		struct lif_interface *bridge = entry->data;

		/* We only care for bridges */
		if (!bridge->is_bridge)
			continue;

		struct lif_dict_entry *bridge_pvid = lif_dict_find(&bridge->vars, "bridge-pvid");
		struct lif_dict_entry *bridge_vids = lif_dict_find(&bridge->vars, "bridge-vids");

		/* If there's nothing to inherit here, carry on */
		if (bridge_pvid == NULL && bridge_vids == NULL)
			continue;

		struct lif_dict_entry *bridge_ports_entry = lif_dict_find(&bridge->vars, "bridge-ports");

		/* This SHOULD not happen, but better save than sorry */
		if (bridge_ports_entry == NULL)
			continue;

		char bridge_ports_str[4096] = {};
		strlcpy(bridge_ports_str, bridge_ports_entry->data, sizeof bridge_ports_str);

		/* If there are no bridge-ports configured, carry on */
		if (strcmp(bridge_ports_str, "none") == 0)
			continue;

		/* Loop over all bridge-ports and set bridge-pvid and bridge-vid if not set already */
		char *bufp = bridge_ports_str;
		for (char *tokenp = lif_next_token(&bufp); *tokenp; tokenp = lif_next_token(&bufp))
		{
			entry = lif_dict_find(collection, tokenp);

			/* There might be interfaces give within the bridge-ports for which there is no
			 * interface stanza. If this is the case, we add one, so we can inherit the
			 * bridge-vids/pvid to it. */
			struct lif_interface *bridge_port;
			if (entry)
				bridge_port = entry->data;

			else if (lif_config.compat_create_interfaces)
			{
				bridge_port = lif_interface_collection_find(collection, tokenp);
				if (bridge_port == NULL)
				{
					fprintf(stderr, "Failed to add interface \"%s\"", tokenp);
					return false;
				}
			}

			/* We would have to creaet an interface, but shouldn't */
			else
			{
				fprintf(stderr, "compat: Missing interface stanza for bridge-port \"%s\" but should not create one.\n",
				        tokenp);
				continue;
			}

			/* Maybe pimp bridge-pvid */
			struct lif_dict_entry *port_pvid = lif_dict_find(&bridge_port->vars, "bridge-pvid");
			if (bridge_pvid && !port_pvid)
				lif_dict_add(&bridge_port->vars, "bridge-pvid", bridge_pvid->data);

			/* Maybe pimp bridge-vids */
			struct lif_dict_entry *port_vids = lif_dict_find(&bridge_port->vars, "bridge-vids");
			if (bridge_vids && !port_vids)
				lif_dict_add(&bridge_port->vars, "bridge-vids", bridge_vids->data);
		}
	}

	return true;
}

bool
lif_compat_apply(struct lif_dict *collection)
{
	if (lif_config.compat_ifupdown2_bridge_ports_inherit_vlans &&
	    !compat_ifupdown2_bridge_ports_inherit_vlans(collection))
		return false;

	return true;
}
