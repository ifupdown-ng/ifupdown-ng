/*
 * libifupdown/libifupdown.h
 * Purpose: main header file for libifupdown
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

#ifndef LIBIFUPDOWN_LIBIFUPDOWN_H__GUARD
#define LIBIFUPDOWN_LIBIFUPDOWN_H__GUARD

#include "libifupdown/list.h"
#include "libifupdown/dict.h"
#include "libifupdown/interface.h"
#include "libifupdown/interface-file.h"
#include "libifupdown/fgetline.h"
#include "libifupdown/version.h"
#include "libifupdown/state.h"
#include "libifupdown/environment.h"
#include "libifupdown/execute.h"
#include "libifupdown/lifecycle.h"
#include "libifupdown/tokenize.h"

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(x)   (sizeof(x) / sizeof(*x))
#endif

#endif
