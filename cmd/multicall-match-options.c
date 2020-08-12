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

static void handle_match(int short_opt, const struct if_option *opt, const char *opt_arg, const struct if_applet *applet)
{
	(void) opt;
	(void) applet;

	switch (short_opt)
	{
	case 'a':
		match_opts.is_auto = true;
		break;
	case 'I':
		match_opts.include_pattern = opt_arg;
		break;
	case 'X':
		match_opts.exclude_pattern = opt_arg;
		break;
	default:
		break;
	}
}

static struct if_option match_options[] = {
	{'a', "auto", NULL, "only match against interfaces hinted as 'auto'", false, handle_match},
	{'I', "include", "include PATTERN", "only match against interfaces matching PATTERN", true, handle_match},
	{'X', "exclude", "exclude PATTERN", "never match against interfaces matching PATTERN", true, handle_match},
};

struct if_option_group match_option_group = {
	.desc = "Matching interfaces",
	.group_size = ARRAY_SIZE(match_options),
	.group = match_options
};
