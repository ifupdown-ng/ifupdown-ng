/*
 * libifupdown/execute.h
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

#ifndef LIBIFUPDOWN_EXECUTE_H__GUARD
#define LIBIFUPDOWN_EXECUTE_H__GUARD

#include <stdarg.h>
#include <stdbool.h>

struct lif_execute_opts {
	bool verbose;
	bool mock;
	bool no_lock;
	bool force;
	const char *executor_path;
	const char *interfaces_file;
	const char *state_file;
};

extern bool lif_execute_fmt(const struct lif_execute_opts *opts, char *const envp[], const char *fmt, ...);
extern bool lif_execute_fmt_with_result(const struct lif_execute_opts *opts, char *buf, size_t bufsize, char *const envp[], const char *fmt, ...);
extern bool lif_file_is_executable(const char *path);
extern bool lif_maybe_run_executor(const struct lif_execute_opts *opts, char *const envp[], const char *executor, const char *phase, const char *lifname);
extern bool lif_maybe_run_executor_with_result(const struct lif_execute_opts *opts, char *const envp[], const char *executor, char *buf, size_t bufsize, const char *phase, const char *lifname);

#endif
