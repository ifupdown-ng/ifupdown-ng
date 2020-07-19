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

#include <stdio.h>
#include "libifupdown/interface-file.h"

bool
lif_interface_file_parse(struct lif_dict *collection, const char *filename)
{
	lif_interface_collection_init(collection);

	return true;
}
