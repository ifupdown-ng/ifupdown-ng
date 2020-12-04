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
	printf(PACKAGE_NAME " " PACKAGE_VERSION "\n"
	       "\n"
	       "Copyright (c) 2020 Ariadne Conill <ariadne@dereferenced.org>\n"
	       "Copyright (c) 2020 Maximilian Wilhelm <max@sdn.clinic>\n"
	       "\n"
	       "Permission to use, copy, modify, and/or distribute this software for any\n"
	       "purpose with or without fee is hereby granted, provided that the above\n"
	       "copyright notice and this permission notice appear in all copies.\n"
	       "\n"
	       "This software is provided 'as is' and without any warranty, express or\n"
	       "implied.  In no event shall the authors be liable for any damages arising\n"
	       "from the use of this software.\n"
	       "\n"
	       "Report bugs at <" PACKAGE_BUGREPORT ">.\n");

	exit(EXIT_SUCCESS);
}
