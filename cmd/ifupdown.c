/*
 * cmd/ifupdown.c
 * Purpose: bring interfaces up or down
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

#define _GNU_SOURCE
#include <fnmatch.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "libifupdown/libifupdown.h"
#include "cmd/multicall.h"

static bool up;
static struct lif_execute_opts exec_opts = {
	.executor_path = EXECUTOR_PATH,
	.interfaces_file = INTERFACES_FILE,
	.state_file = STATE_FILE,
};

void
ifupdown_usage(int status)
{
	fprintf(stderr, "usage: %s [options] <interfaces>\n", argv0);

	fprintf(stderr, "\nOptions:\n");
	fprintf(stderr, "  -h, --help                   this help\n");
	fprintf(stderr, "  -V, --version                show this program's version\n");
	fprintf(stderr, "  -i, --interfaces FILE        use FILE for interface definitions\n");
	fprintf(stderr, "  -S, --state-file FILE        use FILE for state\n");
	fprintf(stderr, "  -a, --auto                   only match against interfaces hinted as 'auto'\n");
	fprintf(stderr, "  -I, --include PATTERN        only match against interfaces matching PATTERN\n");
	fprintf(stderr, "  -X, --exclude PATTERN        never match against interfaces matching PATTERN\n");
	fprintf(stderr, "  -n, --no-act                 do not actually run any commands\n");
	fprintf(stderr, "  -v, --verbose                show what commands are being run\n");
	fprintf(stderr, "  -E, --executor-path PATH     use PATH for executor directory\n");
	fprintf(stderr, "  -f, --force                  force (de)configuration\n");
	fprintf(stderr, "  -L, --no-lock                do not use a lockfile to serialize state changes\n");

	exit(status);
}

bool
is_ifdown()
{
	if (strstr(argv0, "ifdown") != NULL)
		return true;

	return false;
}

int
acquire_state_lock(const char *state_path, const char *lifname)
{
	if (exec_opts.mock || exec_opts.no_lock)
		return -1;

	char lockpath[4096] = {};

	snprintf(lockpath, sizeof lockpath, "%s.%s.lock", state_path, lifname);

	int fd = open(lockpath, O_CREAT | O_WRONLY | O_TRUNC);
	if (fd < 0)
	{
		if (exec_opts.verbose)
			fprintf(stderr, "%s: while opening lockfile %s: %s\n", argv0, lockpath, strerror(errno));
		return -2;
	}

	int flags = fcntl(fd, F_GETFD);
	if (flags < 0)
	{
		close(fd);

		if (exec_opts.verbose)
			fprintf(stderr, "%s: while getting flags for lockfile: %s\n", argv0, strerror(errno));
		return -2;
	}

	flags |= FD_CLOEXEC;
	if (fcntl(fd, F_SETFD, flags) == -1)
	{
		close(fd);

		if (exec_opts.verbose)
			fprintf(stderr, "%s: while setting lockfile close-on-exec: %s\n", argv0, strerror(errno));
		return -2;
	}

	struct flock fl = {
		.l_type = F_WRLCK,
		.l_whence = SEEK_SET
	};

	if (exec_opts.verbose)
		fprintf(stderr, "%s: acquiring lock on %s\n", argv0, lockpath);

	if (fcntl(fd, F_SETLK, &fl) == -1)
	{
		close(fd);

		if (exec_opts.verbose)
			fprintf(stderr, "%s: while locking lockfile: %s\n", argv0, strerror(errno));
		return -2;
	}

	return fd;
}

bool
change_interface(struct lif_interface *iface, struct lif_dict *collection, struct lif_dict *state, const char *ifname)
{
	int lockfd = acquire_state_lock(exec_opts.state_file, ifname);

	if (lockfd == -2)
	{
		fprintf(stderr, "%s: could not acquire exclusive lock for %s: %s\n", argv0, ifname, strerror(errno));
		return false;
	}

	if (exec_opts.verbose)
	{
		fprintf(stderr, "%s: changing state of interface %s to '%s'\n",
			argv0, ifname, up ? "up" : "down");
	}

	if (!lif_lifecycle_run(&exec_opts, iface, collection, state, ifname, up))
	{
		fprintf(stderr, "%s: failed to change interface %s state to '%s'\n",
			argv0, ifname, up ? "up" : "down");

		if (lockfd != -1)
			close(lockfd);

		return false;
	}

	if (lockfd != -1)
		close(lockfd);

	return true;
}

bool
change_auto_interfaces(struct lif_dict *collection, struct lif_dict *state, struct match_options *opts)
{
	struct lif_node *iter;

	LIF_DICT_FOREACH(iter, collection)
	{
		struct lif_dict_entry *entry = iter->data;
		struct lif_interface *iface = entry->data;

		if (opts->is_auto && !iface->is_auto)
			continue;

		if (opts->exclude_pattern != NULL &&
		    !fnmatch(opts->exclude_pattern, iface->ifname, 0))
			continue;

		if (opts->include_pattern != NULL &&
		    fnmatch(opts->include_pattern, iface->ifname, 0))
			continue;

		if (!change_interface(iface, collection, state, iface->ifname))
			return false;
	}

	return true;
}

int
ifupdown_main(int argc, char *argv[])
{
	up = !is_ifdown();

	struct lif_dict state = {};
	struct lif_dict collection = {};
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'V'},
		{"interfaces", required_argument, 0, 'i'},
		{"auto", no_argument, 0, 'a'},
		{"include", required_argument, 0, 'I'},
		{"exclude", required_argument, 0, 'X'},
		{"state-file", required_argument, 0, 'S'},
		{"no-act", no_argument, 0, 'n'},
		{"verbose", no_argument, 0, 'v'},
		{"executor-path", required_argument, 0, 'E'},
		{"force", no_argument, 0, 'f'},
		{"no-lock", no_argument, 0, 'L'},
		{NULL, 0, 0, 0}
	};

	for (;;)
	{
		int c = getopt_long(argc, argv, "hVi:aI:X:S:nvE:fL", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			ifupdown_usage(EXIT_SUCCESS);
			break;
		case 'V':
			lif_common_version();
			break;
		case 'i':
			exec_opts.interfaces_file = optarg;
			break;
		case 'a':
			match_opts.is_auto = true;
			break;
		case 'I':
			match_opts.include_pattern = optarg;
			break;
		case 'X':
			match_opts.exclude_pattern = optarg;
			break;
		case 'S':
			exec_opts.state_file = optarg;
			break;
		case 'n':
			exec_opts.mock = true;
			exec_opts.verbose = true;
			break;
		case 'v':
			exec_opts.verbose = true;
			break;
		case 'E':
			exec_opts.executor_path = optarg;
			break;
		case 'f':
			break;
		case 'L':
			exec_opts.no_lock = true;
			break;
		}
	}

	if (!lif_state_read_path(&state, exec_opts.state_file))
	{
		fprintf(stderr, "%s: could not parse %s\n", argv0, exec_opts.state_file);
		return EXIT_FAILURE;
	}

	if (!lif_interface_file_parse(&collection, exec_opts.interfaces_file))
	{
		fprintf(stderr, "%s: could not parse %s\n", argv0, exec_opts.interfaces_file);
		return EXIT_FAILURE;
	}

	if (!lif_state_sync(&state, &collection))
	{
		fprintf(stderr, "%s: could not sync state\n", argv0);
		return EXIT_FAILURE;
	}

	if (match_opts.is_auto)
	{
		if (!change_auto_interfaces(&collection, &state, &match_opts))
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}
	else if (optind >= argc)
		ifupdown_usage(EXIT_FAILURE);

	int idx = optind;
	for (; idx < argc; idx++)
	{
		char lifbuf[4096];
		strlcpy(lifbuf, argv[idx], sizeof lifbuf);

		char *ifname = lifbuf;
		char *lifname = lifbuf;
		char *p;

		if ((p = strchr(lifbuf, '=')) != NULL)
		{
			*p++ = '\0';
			lifname = p;
		}

		struct lif_interface *iface = lif_state_lookup(&state, &collection, argv[idx]);
		if (iface == NULL)
		{
			struct lif_dict_entry *entry = lif_dict_find(&collection, lifname);

			if (entry == NULL)
			{
				fprintf(stderr, "%s: unknown interface %s\n", argv0, argv[idx]);
				return EXIT_FAILURE;
			}

			iface = entry->data;
		}

		if (!change_interface(iface, &collection, &state, ifname))
			return EXIT_FAILURE;
	}

	if (!exec_opts.mock && !lif_state_write_path(&state, exec_opts.state_file))
	{
		fprintf(stderr, "%s: could not update %s\n", argv0, exec_opts.state_file);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

struct if_applet ifup_applet = {
	.name = "ifup",
	.main = ifupdown_main,
	.usage = ifupdown_usage
};

struct if_applet ifdown_applet = {
	.name = "ifdown",
	.main = ifupdown_main,
	.usage = ifupdown_usage
};
