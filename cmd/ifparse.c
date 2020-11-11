/*
 * cmd/ifparse.c
 * Purpose: Redisplay /e/n/i in alternative formats.
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

#ifdef CONFIG_YAML
# include "libifupdown/yaml-base.h"
# include "libifupdown/yaml-writer.h"
#endif

#include "cmd/multicall.h"
#include "cmd/pretty-print-iface.h"

static bool show_all = false;
static bool allow_undefined = false;

static void
set_show_all(const char *arg)
{
	(void) arg;

	show_all = true;
}

static void
set_allow_undefined(const char *arg)
{
	(void) arg;

	allow_undefined = true;
}

#ifdef CONFIG_YAML
static bool use_yaml = false;

static void
set_yaml(const char *arg)
{
	(void) arg;

	use_yaml = true;
}
#endif

static struct if_option local_options[] = {
	{'A', "all", NULL, "show all interfaces", false, set_show_all},
	{'U', "allow-undefined", NULL, "allow querying undefined (virtual) interfaces", false, set_allow_undefined},
#ifdef CONFIG_YAML
	{'Y', "yaml-raw", NULL, "reflect raw {iface, key, value} triples as YAML", false, set_yaml},
#endif
};

static struct if_option_group local_option_group = {
	.desc = "Program-specific options",
	.group_size = ARRAY_SIZE(local_options),
	.group = local_options
};

#ifdef CONFIG_YAML
static void
prettyprint_interface_yaml(struct lif_interface *iface)
{
	struct lif_yaml_node doc = {};

	lif_yaml_document_init(&doc, "interfaces");

	struct lif_yaml_node *iface_node = lif_yaml_node_new_list(iface->ifname);
	lif_yaml_node_append_child(&doc, iface_node);

	struct lif_node *iter;
	LIF_DICT_FOREACH(iter, &iface->vars)
	{
		struct lif_dict_entry *entry = iter->data;
		const char *value = entry->data;
		char addr_buf[512];

		if (!strcmp(entry->key, "address"))
		{
			struct lif_address *addr = entry->data;

			if (!lif_address_unparse(addr, addr_buf, sizeof addr_buf, true))
				continue;

			value = addr_buf;
		}

		struct lif_yaml_node *iface_entry_node = lif_yaml_node_new_string(entry->key, value);
		lif_yaml_node_append_child(iface_node, iface_entry_node);
	}

	lif_yaml_write(iface_node, stdout);
	lif_yaml_node_free(&doc);
}
#endif

int
ifparse_main(int argc, char *argv[])
{
	struct lif_dict state = {};
	struct lif_dict collection = {};
	struct lif_interface_file_parse_state parse_state = {
		.collection = &collection,
	};

	lif_interface_collection_init(&collection);

	if (!lif_state_read_path(&state, exec_opts.state_file))
	{
		fprintf(stderr, "%s: could not parse %s\n", argv0, exec_opts.state_file);
		return EXIT_FAILURE;
	}

	if (!lif_interface_file_parse(&parse_state, exec_opts.interfaces_file))
	{
		fprintf(stderr, "%s: could not parse %s\n", argv0, exec_opts.interfaces_file);
		return EXIT_FAILURE;
	}

	if (match_opts.property == NULL && lif_lifecycle_count_rdepends(&exec_opts, &collection) == -1)
	{
		fprintf(stderr, "%s: could not validate dependency tree\n", argv0);
		return EXIT_FAILURE;
	}

	if (!lif_compat_apply(&collection))
	{
		fprintf(stderr, "%s: failed to apply compatibility glue\n", argv0);
		return EXIT_FAILURE;
	}

	if (show_all)
	{
		struct lif_node *n;

		LIF_DICT_FOREACH(n, &collection)
		{
			struct lif_dict_entry *entry = n->data;

#ifdef CONFIG_YAML
			if (use_yaml)
				prettyprint_interface_yaml(entry->data);
			else
#endif
				prettyprint_interface_eni(entry->data);
		}

		return EXIT_SUCCESS;
	}

	if (optind >= argc)
		generic_usage(self_applet, EXIT_FAILURE);

	int idx = optind;
	for (; idx < argc; idx++)
	{
		struct lif_dict_entry *entry = lif_dict_find(&collection, argv[idx]);
		struct lif_interface *iface = NULL;

		if (entry != NULL)
			iface = entry->data;

		if (entry == NULL && allow_undefined)
			iface = lif_interface_collection_find(&collection, argv[idx]);

		if (iface == NULL)
		{
			fprintf(stderr, "%s: unknown interface %s\n", argv0, argv[idx]);
			return EXIT_FAILURE;
		}

#ifdef CONFIG_YAML
		if (use_yaml)
			prettyprint_interface_yaml(entry->data);
		else
#endif
			prettyprint_interface_eni(iface);
	}

	return EXIT_SUCCESS;
}

struct if_applet ifparse_applet = {
	.name = "ifparse",
	.desc = "redisplay interface configuration",
	.main = ifparse_main,
	.usage = "ifparse [options] <interfaces>\n  ifquery [options] --all",
	.manpage = "8 ifparse",
	.groups = { &global_option_group, &match_option_group, &exec_option_group, &local_option_group },
};
