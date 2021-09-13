/*
 * cmd/ifctrstat-linux.c
 * Purpose: Implement ifctrstat system-specific routines for Linux
 *
 * Copyright (c) 2020 Adélie Software in the Public Benefit, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "cmd/multicall.h"
#include "cmd/ifctrstat-linux.h"

struct counter_desc {
	const char *name;
	const void *data;
} avail_counters[] = {
	{"rx.discard", "rx_dropped"},
	{"rx.errors", "rx_errors"},
	{"rx.octets", "rx_bytes"},
	{"rx.packets", "rx_packets"},
	{"tx.discard", "tx_dropped"},
	{"tx.errors", "tx_errors"},
	{"tx.octets", "tx_bytes"},
	{"tx.packets", "tx_packets"}
};

size_t avail_counters_count = ARRAY_SIZE(avail_counters);

static int
counter_compare(const void *key, const void *candidate)
{
	return strcasecmp((const char *)key, ((struct counter_desc *)candidate)->name);
}

const char *
read_counter(const char *interface, const char *counter)
{
	FILE *fp;
	const char *path;
	char full_path[PATH_MAX];
	char buffer[1024];
	size_t in_count;
	struct counter_desc *ctrdata;

	errno = 0;

	ctrdata = bsearch(counter, avail_counters, avail_counters_count, sizeof(struct counter_desc), counter_compare);
	if (ctrdata) {
		path = (const char *)ctrdata->data;
	} else {
		errno = ENOSYS;
		return NULL;
	}

	if (snprintf(full_path, PATH_MAX, "/sys/class/net/%s/statistics/%s", interface, path) > PATH_MAX)
	{
		errno = ENOMEM;
		return NULL;
	}

	fp = fopen(full_path, "r");
	if (!fp)
	{
		return NULL;
	}

	in_count = fread(buffer, 1, sizeof(buffer), fp);

	if (in_count == sizeof(buffer))
	{
		errno = ENOMEM;
		fclose(fp);
		return NULL;
	}

	if (ferror(fp))
	{
		return NULL;
	}

	fclose(fp);

	/* take away the \n, we add our own */
	buffer[in_count - 1] = '\0';

	return strdup(buffer);
}
