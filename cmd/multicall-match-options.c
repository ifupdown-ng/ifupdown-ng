/*
 * cmd/multicall-match-options.c
 * Purpose: multi-call binary frontend -- option handling
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
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "cmd/multicall.h"

struct match_options match_opts = {};

static void
set_auto(const char *opt_arg)
{
	(void) opt_arg;
	match_opts.is_auto = true;
}

static void
set_include_pattern(const char *opt_arg)
{
	match_opts.include_pattern = opt_arg;
}

static void
set_exclude_pattern(const char *opt_arg)
{
	match_opts.include_pattern = opt_arg;
}

static struct if_option match_options[] = {
	{'a', "auto", NULL, "only match against interfaces hinted as 'auto'", false, set_auto},
	{'I', "include", "include PATTERN", "only match against interfaces matching PATTERN", true, set_include_pattern},
	{'X', "exclude", "exclude PATTERN", "never match against interfaces matching PATTERN", true, set_exclude_pattern},
};

struct if_option_group match_option_group = {
	.desc = "Matching interfaces",
	.group_size = ARRAY_SIZE(match_options),
	.group = match_options
};
