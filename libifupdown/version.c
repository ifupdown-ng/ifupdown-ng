/*
 * libifupdown/version.c
 * Purpose: lif_common_version() header
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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "libifupdown/version.h"

void
lif_common_version(void)
{
	printf("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);

	printf("\nCopyright (c) 2020 Ariadne Conill <ariadne@dereferenced.org>\n");
	printf("Copyright (c) 2020 Maximilian Wilhelm <max@sdn.clinic>\n\n");

	printf("Permission to use, copy, modify, and/or distribute this software for any\n");
	printf("purpose with or without fee is hereby granted, provided that the above\n");
	printf("copyright notice and this permission notice appear in all copies.\n\n");

	printf("This software is provided 'as is' and without any warranty, express or\n");
	printf("implied.  In no event shall the authors be liable for any damages arising\n");
	printf("from the use of this software.\n\n");

	printf("Report bugs at <%s>.\n", PACKAGE_BUGREPORT);

	exit(EXIT_SUCCESS);
}
