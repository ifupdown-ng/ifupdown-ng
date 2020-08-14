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
#include "multicall.h"

const char *avail_counters[] = {
	"rx.octets",
	"rx.packets",
	"rx.discard",
	"rx.errors",
	"tx.octets",
	"tx.packets",
	"tx.discard",
	"tx.errors"
};

size_t avail_counters_count = ARRAY_SIZE(avail_counters);

char *
read_counter(const char *interface, const char *counter)
{
	FILE *fp;
	const char *path;
	char full_path[PATH_MAX];
	char buffer[1024];
	size_t in_count;

	errno = 0;

	if (strcasecmp(counter, "rx.octets") == 0)
	{
		path = "rx_bytes";
	} else if (strcasecmp(counter, "rx.packets") == 0) {
		path = "rx_packets";
	} else if (strcasecmp(counter, "rx.discard") == 0) {
		path = "rx_dropped";
	} else if (strcasecmp(counter, "rx.errors") == 0) {
		path = "rx_errors";
	} else if (strcasecmp(counter, "tx.octets") == 0) {
		path = "tx_bytes";
	} else if (strcasecmp(counter, "tx.packets") == 0) {
		path = "tx_packets";
	} else if (strcasecmp(counter, "tx.discard") == 0) {
		path = "tx_dropped";
	} else if (strcasecmp(counter, "tx.errors") == 0) {
		path = "tx_errors";
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
