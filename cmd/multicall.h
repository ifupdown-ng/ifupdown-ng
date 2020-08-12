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
	int (*const main)(int argc, char *argv[]);
	void (*const usage)(int status);
	const struct if_option_group *groups[4];
};

extern char *argv0;
extern struct if_option_group global_option_group;

struct match_options {
	bool is_auto;
	char *exclude_pattern;
	char *include_pattern;
	bool pretty_print;
	bool dot;
	char *property;
};

#endif
