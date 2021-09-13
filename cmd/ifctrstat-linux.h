/*
 * cmd/ifctrstat-linux.c
 * Purpose: Implement ifctrstat system-specific routines for Linux
 *
 * Copyright (c) 2021 Maximilian Wilhelm <max@sdn.clinic>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */


#ifndef IFUPDOWN_IFCTRSTAT_LINUX__H__GUARD
#define IFUPDOWN_IFCTRSTAT_LINUX__H__GUARD

extern const char * read_counter(const char *interface, const char *counter);

#endif
