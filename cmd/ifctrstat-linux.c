/*
 * cmd/ifctrstat-linux.c
 * Purpose: Implement ifctrstat system-specific routines for Linux
 *
 * Copyright (c) 2020 Adélie Software in the Public Benefit, Inc.
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
#include <stdio.h>
#include "multicall.h"

const char *avail_counters[] = {
	"rx.octets",
	"rx.packets",
	"rx.discard",
	"rx.errors",
	"tx.octets",
	"tx.packets",
	"tx.discard",
	"tx.errors"
};

size_t avail_counters_count = ARRAY_SIZE(avail_counters);

const char *
read_counter(const char *interface, const char *counter)
{
	errno = ENOSYS;
	return NULL;
}
