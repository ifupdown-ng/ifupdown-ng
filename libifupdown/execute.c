/*
 * libifupdown/execute.c
 * Purpose: execution of individual commands
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

#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "libifupdown/execute.h"

#define SHELL	"/bin/sh"

bool
lif_execute_fmt(char *const envp[], const char *fmt, ...)
{
	char cmdbuf[4096];
	va_list va;

	va_start(va, fmt);
	vsnprintf(cmdbuf, sizeof cmdbuf, fmt, va);
	va_end(va);

	pid_t child;
	char *argv[] = { SHELL, "-c", cmdbuf, NULL };

	if (posix_spawn(&child, SHELL, NULL, NULL, argv, envp) != 0)
	{
		fprintf(stderr, "execute '%s': %s\n", cmdbuf, strerror(errno));
		return false;
	}

	int status;
	waitpid(child, &status, 0);

	return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}
