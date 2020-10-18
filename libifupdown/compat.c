/*
 * libifupdown/compat.c
 * Purpose: compatiblity glue to other implementations
 *
 * Copyright (c) 2020 Maximilian Wilhelm <max@sdn.clinic>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#include <stdbool.h>
#include "libifupdown/dict.h"

extern bool
lif_compat_apply(struct lif_dict *collection)
{
	(void) collection;

	/* Mangle interfaces according to some config options here */

	return true;
}
