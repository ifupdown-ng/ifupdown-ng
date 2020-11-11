/*
 * cmd/pretty-print-iface.h
 * Purpose: interface pretty-printer (/e/n/i style)
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

#ifndef IFUPDOWN_CMD_PRETTY_PRINT_IFACE_H__GUARD
#define IFUPDOWN_CMD_PRETTY_PRINT_IFACE_H__GUARD

#include "libifupdown/libifupdown.h"

extern void prettyprint_interface_eni(struct lif_interface *iface);

#endif
