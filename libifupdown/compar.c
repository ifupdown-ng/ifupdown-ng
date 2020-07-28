/*
 * libifupdown/compar.c
 * Purpose: comparators
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

#include <string.h>
#include "libifupdown/compar.h"

int
compar_str (const void *a, const void *b)
{
	const char *str_a = (const char *)a;
	const char *str_b = (const char *)b;

	return strcmp (str_a, str_b);
}
