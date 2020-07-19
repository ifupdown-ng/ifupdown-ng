/*
 * libifupdown/fgetline.h
 * Purpose: portable fgetline(3)
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

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifndef LIBIFUPDOWN_FGETLINE_H__GUARD
#define LIBIFUPDOWN_FGETLINE_H__GUARD

extern char *lif_fgetline(char *line, size_t size, FILE *stream);

#endif
