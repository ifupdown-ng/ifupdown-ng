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
#include "cmd/multicall.h"

void
print_interface(struct lif_interface *iface)
{
	if (!lif_lifecycle_query_dependents(&exec_opts, iface, iface->ifname))
		return;

	if (iface->is_auto)
		printf("auto %s\n", iface->ifname);

	printf("%s %s\n", iface->is_template ? "template" : "iface", iface->ifname);

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

void
print_interface_dot(struct lif_dict *collection, struct lif_interface *iface, struct lif_interface *parent)
{
	if (!lif_lifecycle_query_dependents(&exec_opts, iface, iface->ifname))
		return;

	if (parent != NULL)
		printf("\"%s (%zu)\" -> ", parent->ifname, parent->rdepends_count);

	printf("\"%s (%zu)\"", iface->ifname, iface->rdepends_count);

	printf("\n");

	struct lif_dict_entry *entry = lif_dict_find(&iface->vars, "requires");

	if (entry == NULL)
		return;

	char require_ifs[4096] = {};
	strlcpy(require_ifs, entry->data, sizeof require_ifs);
	char *reqp = require_ifs;

	for (char *tokenp = lif_next_token(&reqp); *tokenp; tokenp = lif_next_token(&reqp))
	{
		struct lif_interface *child_if = lif_interface_collection_find(collection, tokenp);

		if (child_if->is_pending)
			continue;

		child_if->is_pending = true;
		print_interface_dot(collection, child_if, iface);
		child_if->is_pending = false;
	}
}

void
print_interface_property(struct lif_interface *iface, const char *property)
{
	struct lif_node *iter;
	bool printing_address = !strcmp(property, "address");

	LIF_DICT_FOREACH(iter, &iface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (strcmp(entry->key, property))
			continue;

		if (printing_address)
		{
			char addr_buf[512];
			if (!lif_address_format_cidr(iface, entry, addr_buf, sizeof(addr_buf)))
				continue;

			printf("%s\n", addr_buf);
		}
		else
			printf("%s\n", (const char *) entry->data);
	}
}

void
list_interfaces(struct lif_dict *collection, struct match_options *opts)
{
	struct lif_node *iter;

	if (opts->dot)
	{
		printf("digraph interfaces {\n");
		printf("edge [color=blue fontname=Sans fontsize=10]\n");
		printf("node [fontname=Sans fontsize=10]\n");
	}

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

		if (opts->pretty_print)
			print_interface(iface);
		else if (opts->dot)
			print_interface_dot(collection, iface, NULL);
		else
			printf("%s\n", iface->ifname);
	}

	if (opts->dot)
		printf("}\n");
}

static bool listing = false, listing_stat = false, listing_running = false;
static bool allow_undefined = false;

void
list_state(struct lif_dict *state, struct match_options *opts)
{
	struct lif_node *iter;

	LIF_DICT_FOREACH(iter, state)
	{
		struct lif_dict_entry *entry = iter->data;

		if (opts->exclude_pattern != NULL &&
		    !fnmatch(opts->exclude_pattern, entry->key, 0))
			continue;

		if (opts->include_pattern != NULL &&
		    fnmatch(opts->include_pattern, entry->key, 0))
			continue;

		struct lif_state_record *rec = entry->data;

		if (listing_running)
			printf("%s\n", entry->key);
		else
			printf("%s=%s %zu%s\n", entry->key, rec->mapped_if, rec->refcount,
			       rec->is_explicit ? " explicit" : "");
	}
}

static void
set_listing(const char *opt_arg)
{
	(void) opt_arg;
	listing = true;
}

static void
set_show_state(const char *opt_arg)
{
	(void) opt_arg;
	listing_stat = true;
}

static void
set_show_running(const char *opt_arg)
{
	(void) opt_arg;
	listing_running = true;
}

static void
set_pretty_print(const char *opt_arg)
{
	(void) opt_arg;
	match_opts.pretty_print = true;
}

static void
set_output_dot(const char *opt_arg)
{
	(void) opt_arg;
	match_opts.dot = true;
}

static void
set_property(const char *opt_arg)
{
	match_opts.property = opt_arg;
}

static void
set_allow_undefined(const char *opt_arg)
{
	(void) opt_arg;
	allow_undefined = true;
}

static struct if_option local_options[] = {
	{'r', "running", NULL, "show configured (running) interfaces", false, set_show_running},
	{'s', "state", NULL, "show configured state", false, set_show_state},
	{'p', "property", "property PROPERTY", "print values of properties matching PROPERTY", true, set_property},
	{'D', "dot", NULL, "generate a dependency graph", false, set_output_dot},
	{'L', "list", NULL, "list matching interfaces", false, set_listing},
	{'P', "pretty-print", NULL, "pretty print the interfaces instead of just listing", false, set_pretty_print},
	{'U', "allow-undefined", NULL, "allow querying undefined (virtual) interfaces", false, set_allow_undefined},
};

static struct if_option_group local_option_group = {
	.desc = "Program-specific options",
	.group_size = ARRAY_SIZE(local_options),
	.group = local_options
};

int
ifquery_main(int argc, char *argv[])
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

	/* --list --state is not allowed */
	if (listing && (listing_stat || listing_running))
		generic_usage(self_applet, EXIT_FAILURE);

	if (listing)
	{
		list_interfaces(&collection, &match_opts);
		return EXIT_SUCCESS;
	}
	else if (listing_stat || listing_running)
	{
		list_state(&state, &match_opts);
		return EXIT_SUCCESS;
	}

	if (optind >= argc)
		generic_usage(self_applet, EXIT_FAILURE);

	int idx = optind;
	for (; idx < argc; idx++)
	{
		struct lif_interface *iface = lif_state_lookup(&state, &collection, argv[idx]);

		if (iface == NULL)
		{
			struct lif_dict_entry *entry = lif_dict_find(&collection, argv[idx]);

			if (entry != NULL)
				iface = entry->data;

			if (entry == NULL && allow_undefined)
				iface = lif_interface_collection_find(&collection, argv[idx]);
		}

		if (iface == NULL)
		{
			fprintf(stderr, "%s: unknown interface %s\n", argv0, argv[idx]);
			return EXIT_FAILURE;
		}

		if (match_opts.property != NULL)
			print_interface_property(iface, match_opts.property);
		else
			print_interface(iface);
	}

	return EXIT_SUCCESS;
}

struct if_applet ifquery_applet = {
	.name = "ifquery",
	.desc = "query interface configuration",
	.main = ifquery_main,
	.usage = "ifquery [options] <interfaces>\n  ifquery [options] --list",
	.manpage = "8 ifquery",
	.groups = { &global_option_group, &match_option_group, &exec_option_group, &local_option_group },
};
