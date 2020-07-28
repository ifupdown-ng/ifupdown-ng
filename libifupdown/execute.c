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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>

#include "libifupdown/execute.h"

#define SHELL	"/bin/sh"

bool
lif_execute_fmt(const struct lif_execute_opts *opts, char *const envp[], const char *fmt, ...)
{
	char cmdbuf[4096];
	va_list va;

	va_start(va, fmt);
	vsnprintf(cmdbuf, sizeof cmdbuf, fmt, va);
	va_end(va);

	pid_t child;
	char *argv[] = { SHELL, "-c", cmdbuf, NULL };

	if (opts->verbose)
		puts(cmdbuf);

	if (opts->mock)
		return true;

	if (posix_spawn(&child, SHELL, NULL, NULL, argv, envp) != 0)
	{
		fprintf(stderr, "execute '%s': %s\n", cmdbuf, strerror(errno));
		return false;
	}

	int status;
	waitpid(child, &status, 0);

	return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

bool
lif_execute_fmt_with_result(const struct lif_execute_opts *opts, char *buf, size_t bufsize, char *const envp[], const char *fmt, ...)
{
	char cmdbuf[4096];
	va_list va;

	va_start(va, fmt);
	vsnprintf(cmdbuf, sizeof cmdbuf, fmt, va);
	va_end(va);

	pid_t child;
	char *argv[] = { SHELL, "-c", cmdbuf, NULL };

	if (opts->verbose)
		puts(cmdbuf);

	if (opts->mock)
		return true;

	int pipefds[2];
	if (pipe(pipefds) < 0)
	{
		fprintf(stderr, "execute '%s': %s\n", cmdbuf, strerror(errno));
		return false;
	}

	posix_spawn_file_actions_t file_actions;

	posix_spawn_file_actions_init(&file_actions);
	posix_spawn_file_actions_addclose(&file_actions, pipefds[0]);
	posix_spawn_file_actions_adddup2(&file_actions, pipefds[1], 1);
	posix_spawn_file_actions_addclose(&file_actions, pipefds[1]);

	if (posix_spawn(&child, SHELL, &file_actions, NULL, argv, envp) != 0)
	{
		fprintf(stderr, "execute '%s': %s\n", cmdbuf, strerror(errno));
		return false;
	}

	close(pipefds[1]);

	struct pollfd pfd = {
		.fd = pipefds[0],
		.events = POLLIN
	};

	if (poll(&pfd, 1, -1) < 1)
		goto no_result;

	if (read(pipefds[0], buf, bufsize) < 0)
	{
		fprintf(stderr, "reading from pipe: %s\n", strerror(errno));
		return false;
	}

	int status;
no_result:
	waitpid(child, &status, 0);

	return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

bool
lif_file_is_executable(const char *path)
{
	struct stat st;

	if (stat(path, &st))
		return false;

	if (!S_ISREG(st.st_mode))
		return false;

	return !access(path, X_OK);
}

bool
lif_maybe_run_executor(const struct lif_execute_opts *opts, char *const envp[], const char *executor)
{
	if (opts->verbose)
		fprintf(stderr, "ifupdown: attempting to run %s executor\n", executor);

	char pathbuf[4096];

	snprintf(pathbuf, sizeof pathbuf, "%s/%s", opts->executor_path, executor);

	if (!lif_file_is_executable(pathbuf))
		return true;

	return lif_execute_fmt(opts, envp, "%s", pathbuf);
}

bool
lif_maybe_run_executor_with_result(const struct lif_execute_opts *opts, char *const envp[], const char *executor, char *buf, size_t bufsize)
{
	if (opts->verbose)
		fprintf(stderr, "ifupdown: attempting to run %s executor\n", executor);

	char pathbuf[4096];

	snprintf(pathbuf, sizeof pathbuf, "%s/%s", opts->executor_path, executor);

	if (!lif_file_is_executable(pathbuf))
		return true;

	return lif_execute_fmt_with_result(opts, buf, bufsize, envp, "%s", pathbuf);
}
