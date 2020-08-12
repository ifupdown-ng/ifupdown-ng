/*
 * cmd/multicall-exec-options.c
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

struct lif_execute_opts exec_opts = {
	.interfaces_file = INTERFACES_FILE,
	.executor_path = EXECUTOR_PATH,
	.state_file = STATE_FILE
};

static void handle_exec(int short_opt, const struct if_option *opt, const char *opt_arg, const struct if_applet *applet)
{
	(void) opt;
	(void) applet;

	switch (short_opt)
	{
		case 'i':
			exec_opts.interfaces_file = opt_arg;
			break;
		case 'S':
			exec_opts.state_file = opt_arg;
			break;
		case 'n':
			exec_opts.mock = true;
			exec_opts.verbose = true;
			break;
		case 'v':
			exec_opts.verbose = true;
			break;
		case 'E':
			exec_opts.executor_path = opt_arg;
			break;
		case 'f':
			break;
		case 'L':
			exec_opts.no_lock = true;
			break;
		default:
			break;
	}
}

static struct if_option exec_options[] = {
	{'f', "force", NULL, "force (de)configuration", true, handle_exec},
	{'i', "interfaces", "interfaces FILE", "use FILE for interface definitions", true, handle_exec},
	{'n', "no-act", NULL, "do not actually run any commands", false, handle_exec},
	{'v', "verbose", NULL, "show what commands are being run", false, handle_exec},
	{'E', "executor-path", "executor-path PATH", "use PATH for executor directory", true, handle_exec},
	{'L', "no-lock", NULL, "do not use a lockfile to serialize state changes", false, handle_exec},
	{'S', "state-file", "state-file FILE", "use FILE for state", true, handle_exec},
};

struct if_option_group exec_option_group = {
	.desc = "Execution",
	.group_size = ARRAY_SIZE(exec_options),
	.group = exec_options
};
