/*
 * cmd/crc32.c
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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "cksum.h"

#include "libifupdown/libifupdown.h"
#include "cmd/multicall.h"

static bool output_hex = false;

static void
set_output_hex(const char *opt_arg)
{
        (void) opt_arg;
        output_hex = true;
}

static struct if_option local_options[] = {
        {'x', "hex", NULL, "outputs CRC32 value in hex for the interface.", false, set_output_hex},
};

static struct if_option_group local_option_group = {
        .desc = "Program-specific options",
        .group_size = ARRAY_SIZE(local_options),
        .group = local_options
};

static
int ifcrc32sum_main(int32_t argc, char **argv)
{
    uint_fast32_t crc;
    uintmax_t total_bytes = 0;

    if (optind >= argc)
        generic_usage(self_applet, EXIT_FAILURE);

    int idx = optind;
    if (argc - idx == 0)
    {
        fprintf(stderr, "%s: interface required\n",
            argv0);
        return EXIT_FAILURE;
    }

    const char *iface = argv[idx++];

    crc_sum_string (iface, &crc, &total_bytes);

    if (output_hex)
	printf("%08" PRIxFAST32 "\n", crc);
    else
	printf("%" PRIuFAST32 " " "%" PRIuMAX "\n", crc, total_bytes);

    exit(EXIT_SUCCESS);
}

struct if_applet ifcrc32sum_applet = {
        .name = "ifcrc32sum",
        .desc = "calculate CRC32 value for VRRP MACVLAN subinterfaces.\n\nThis tool is used by VRRP executor to create stable names in system for MACVLAN subinterfaces.\ncksum from coretutils package can be used as an alternative tool. The CRC32 value is the same.",
        .main = ifcrc32sum_main,
        .usage = "ifcrc32sum [options] <interface>",
        .manpage = "8 ifcrc32sum",
        .groups = { &global_option_group, &local_option_group, },
};
APPLET_REGISTER(ifcrc32sum_applet)
