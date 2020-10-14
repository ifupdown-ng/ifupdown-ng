/*
 * libifupdown/interface-file.h
 * Purpose: /etc/network/interfaces parser
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

#ifndef LIBIFUPDOWN_INTERFACE_FILE_H__GUARD
#define LIBIFUPDOWN_INTERFACE_FILE_H__GUARD

#include <stdbool.h>
#include "libifupdown/interface.h"
#include "libifupdown/dict.h"

struct lif_interface_file_parse_state {
	struct lif_interface *cur_iface;
	struct lif_dict *collection;
	const char *cur_filename;
	size_t cur_lineno;

	struct lif_dict loaded;
};

extern bool lif_interface_file_parse(struct lif_interface_file_parse_state *state, const char *filename);

#endif
