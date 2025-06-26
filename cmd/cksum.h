/*
 * cmd/cksum.h
 * Purpose: calculate and print POSIX checksums for a specific string.
 *
 *  This tool is based on cksum tool from Linux coreutils.
 *  Is using the same CRC32 lookup table found in crctab.c and same calculations
 * as cksum from coreutils to output the same value for the same string input.
 *  Is necessary for ifupdown-ng when the cksum tools is not avaiable on
 * a system and we need to generate VRRP names.
 *
 * Copyright (c) 2023 EasynetDev (Adrian Ban) <devel@easynet.dev> in the Public Benefit, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#ifndef __CKSUM_H
#define __CKSUM_H

extern int
crc_sum_string (const char *iface, uint_fast32_t *crc_out, uintmax_t *length);

extern uint_fast32_t const crctab[8][256];

#endif /* __CKSUM_H */
