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

#ifndef IFUPDOWN_CMD_MULTICALL_H__GUARD
#define IFUPDOWN_CMD_MULTICALL_H__GUARD

#include "libifupdown/libifupdown.h"

struct if_applet {
	const char *name;
	int (*const main)(int argc, char *argv[]);
	void (*const usage)(int status);
};

extern char *argv0;

#endif
