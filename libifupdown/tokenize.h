/*
 * libifupdown/tokenize.h
 * Purpose: tokenization helper
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

#ifndef LIBIFUPDOWN_TOKENIZE_H__GUARD
#define LIBIFUPDOWN_TOKENIZE_H__GUARD

#include <ctype.h>

static inline char *
lif_next_token(char **buf)
{
	char *out = *buf;

	while (*out && isspace(*out))
		out++;

	char *end = out;
	while (*end && !isspace(*end))
		end++;

	*end++ = '\0';
	*buf = end;

	return out;
}

#endif
