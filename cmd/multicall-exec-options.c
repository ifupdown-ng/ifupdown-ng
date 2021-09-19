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

#define DEFAULT_TIMEOUT		300

struct lif_execute_opts exec_opts = {
	.interfaces_file = INTERFACES_FILE,
	.executor_path = EXECUTOR_PATH,
	.state_file = STATE_FILE,
	.timeout = DEFAULT_TIMEOUT,
};

static void
set_interfaces_file(const char *opt_arg)
{
	exec_opts.interfaces_file = opt_arg;
}

static void
set_state_file(const char *opt_arg)
{
	exec_opts.state_file = opt_arg;
}

static void
set_no_act(const char *opt_arg)
{
	(void) opt_arg;
	exec_opts.mock = true;
	exec_opts.verbose = true;
}

static void
set_verbose(const char *opt_arg)
{
	(void) opt_arg;
	exec_opts.verbose = true;
}

static void
set_executor_path(const char *opt_arg)
{
	exec_opts.executor_path = opt_arg;
}

static void
set_no_lock(const char *opt_arg)
{
	(void) opt_arg;
	exec_opts.no_lock = true;
}

static void
set_force(const char *opt_arg)
{
	(void) opt_arg;
	exec_opts.force = true;
}

static void
set_timeout(const char *opt_arg)
{
	exec_opts.timeout = atoi(opt_arg);
	if (exec_opts.timeout < 0)
		exec_opts.timeout = DEFAULT_TIMEOUT;
}

static struct if_option exec_options[] = {
	{'f', "force", NULL, "force (de)configuration", false, set_force},
	{'i', "interfaces", "interfaces FILE", "use FILE for interface definitions", true, set_interfaces_file},
	{'l', "no-lock", NULL, "do not use a lockfile to serialize state changes", false, set_no_lock},
	{'n', "no-act", NULL, "do not actually run any commands", false, set_no_act},
	{'v', "verbose", NULL, "show what commands are being run", false, set_verbose},
	{'E', "executor-path", "executor-path PATH", "use PATH for executor directory", true, set_executor_path},
	{'S', "state-file", "state-file FILE", "use FILE for state", true, set_state_file},
	{'T', "timeout", "timeout TIMEOUT", "wait TIMEOUT seconds for executors to complete", true, set_timeout},
};

struct if_option_group exec_option_group = {
	.desc = "Execution",
	.group_size = ARRAY_SIZE(exec_options),
	.group = exec_options
};
