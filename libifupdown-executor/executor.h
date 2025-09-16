/*
 * libifupdown-executor/executor.h
 * Purpose: Native executor API
 *
 * Copyright (c) 2021 Ariadne Conill <ariadne@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#ifndef LIBIFUPDOWN_EXECUTOR_EXECUTOR_H__GUARD
#define LIBIFUPDOWN_EXECUTOR_EXECUTOR_H__GUARD

#include "libifupdown/libifupdown.h"

typedef bool (*lif_executor_phase_fn)(struct lif_interface *iface);

struct lif_executor {
	/* the name of the executor, for logging and so on */
	const char *name;

	/* optional handlers for each phase */
	const lif_executor_phase_fn create;
	const lif_executor_phase_fn pre_up;
	const lif_executor_phase_fn up;
	const lif_executor_phase_fn post_up;
	const lif_executor_phase_fn pre_down;
	const lif_executor_phase_fn down;
	const lif_executor_phase_fn post_down;
	const lif_executor_phase_fn destroy;
	const lif_executor_phase_fn depend;
};

#endif
