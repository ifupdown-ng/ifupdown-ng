/*
 * cmd/multicall.h
 * Purpose: structures for multicall frontend
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

#include <stdbool.h>

#ifndef IFUPDOWN_CMD_MULTICALL_H__GUARD
#define IFUPDOWN_CMD_MULTICALL_H__GUARD

#include "libifupdown/libifupdown.h"

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(*x))

struct if_applet;

struct if_option {
	char short_opt;
	const char *long_opt;
	const char *long_opt_desc;
	const char *desc;
	bool require_argument;
	void (*const handle)(int short_opt, const struct if_option *opt, const char *opt_arg, const struct if_applet *applet);
};

struct if_option_group {
	const char *desc;
	size_t group_size;
	const struct if_option *group;
};

struct if_applet {
	const char *name;
	const char *desc;
	const char *usage;
	int (*const main)(int argc, char *argv[]);
	const struct if_option_group *groups[16];
};

extern char *argv0;
extern const struct if_applet *self_applet;
extern struct if_option_group global_option_group;

struct match_options {
	bool is_auto;
	const char *exclude_pattern;
	const char *include_pattern;
	bool pretty_print;
	bool dot;
	const char *property;
};

extern struct match_options match_opts;

extern void process_options(const struct if_applet *applet, int argc, char *argv[]);
extern const struct if_option *lookup_option(const struct if_applet *applet, int opt);

extern struct if_option_group match_option_group;

extern struct lif_execute_opts exec_opts;
extern struct if_option_group exec_option_group;

void generic_usage(const struct if_applet *applet, int result);

#endif
