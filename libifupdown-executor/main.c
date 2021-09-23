/*
 * libifupdown-executor/main.c
 * Purpose: unified main() function for executors
 *
 * Copyright (c) 2021 Ariadne Conill <ariadne@dereferenced.org>
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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "libifupdown-executor/executor.h"

extern struct lif_executor lif_exec;

#define DEFAULT_TIMEOUT		300

struct lif_execute_opts exec_opts = {
	.interfaces_file = INTERFACES_FILE,
	.executor_path = EXECUTOR_PATH,
	.state_file = STATE_FILE,
	.timeout = DEFAULT_TIMEOUT,
};

struct lif_executor_phase_mapping {
	const char *name;
	const lif_executor_phase_fn *phase_fn;
};

/* keep in alphabetical order for bsearch(3) */
static const struct lif_executor_phase_mapping phase_mappings[] = {
	{"create", &lif_exec.create},
	{"depend", &lif_exec.depend},
	{"destroy", &lif_exec.destroy},
	{"down", &lif_exec.down},
	{"post-down", &lif_exec.post_down},
	{"post-up", &lif_exec.post_up},
	{"pre-down", &lif_exec.pre_down},
	{"pre-up", &lif_exec.pre_up},
	{"up", &lif_exec.up},
};

static int phase_mapping_cmp(const void *key, const void *ptr) {
	const struct lif_executor_phase_mapping *mapping = ptr;
	return strcasecmp(key, mapping->name);
}

int
main(int argc, const char *argv[])
{
	(void) argc;
	(void) argv;

	struct lif_dict state = {};
	struct lif_dict collection = {};
	struct lif_interface_file_parse_state parse_state = {
		.collection = &collection,
	};

	lif_interface_collection_init(&collection);

	if (!lif_state_read_path(&state, exec_opts.state_file))
	{
		fprintf(stderr, "%s: could not parse %s\n", argv[0], exec_opts.state_file);
		return EXIT_FAILURE;
	}

	if (!lif_interface_file_parse(&parse_state, exec_opts.interfaces_file))
	{
		fprintf(stderr, "%s: could not parse %s\n", argv[0], exec_opts.interfaces_file);
		return EXIT_FAILURE;
	}

	if (lif_lifecycle_count_rdepends(&exec_opts, &collection) == -1)
	{
		fprintf(stderr, "%s: could not validate dependency tree\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!lif_compat_apply(&collection))
	{
		fprintf(stderr, "%s: failed to apply compatibility glue\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!lif_state_sync(&state, &collection))
	{
		fprintf(stderr, "%s: could not sync state\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *iface_name = getenv("IFACE");
	if (iface_name == NULL)
	{
		fprintf(stderr, "%s: environmental variable IFACE is not declared\n", argv[0]);
		return EXIT_FAILURE;
	}

	struct lif_interface *iface = lif_interface_collection_find(&collection, iface_name);
	if (iface == NULL)
	{
		fprintf(stderr, "%s: interface %s is not configured\n", argv[0], iface_name);
		return EXIT_FAILURE;
	}

	const char *phase = getenv("PHASE");
	if (phase == NULL)
	{
		fprintf(stderr, "%s: environmental variable PHASE is not declared\n", argv[0]);
		return EXIT_FAILURE;
	}

	const struct lif_executor_phase_mapping *mapping = bsearch(phase, phase_mappings,
		ARRAY_SIZE(phase_mappings), sizeof(struct lif_executor_phase_mapping),
		phase_mapping_cmp);
	if (mapping == NULL)
	{
		fprintf(stderr, "%s: unknown PHASE %s requested\n", argv[0], phase);
		return EXIT_FAILURE;
	}

	const lif_executor_phase_fn phase_fn = *mapping->phase_fn;
	if (phase_fn == NULL)
		return EXIT_SUCCESS;

	return phase_fn(iface) ? EXIT_SUCCESS : EXIT_FAILURE;
}
