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
};

extern bool lif_execute_fmt(const struct lif_execute_opts *opts, char *const envp[], const char *fmt, ...);

#endif
