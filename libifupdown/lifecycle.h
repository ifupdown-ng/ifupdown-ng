/*
 * libifupdown/lifecycle.h
 * Purpose: management of interface lifecycle (bring up, takedown, reload)
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

#ifndef LIBIFUPDOWN_LIFECYCLE_H__GUARD
#define LIBIFUPDOWN_LIFECYCLE_H__GUARD

#include "libifupdown/interface.h"
#include "libifupdown/execute.h"

extern bool lif_lifecycle_run_phase(const struct lif_execute_opts *opts, struct lif_interface *iface, const char *phase, const char *lifname, bool up);
extern bool lif_lifecycle_run(const struct lif_execute_opts *opts, struct lif_interface *iface, struct lif_dict *state, const char *lifname, bool up);

#endif

