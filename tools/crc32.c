/* crc32 -- calculate and print POSIX checksums for a specific string.

   This tool is a stipped clone of cksum tool from Linux coreutils.
   Is using the same crc32 lookup table crctab as coreutils.
   Is necessary for ifupdown-ng when the cksum tools is not avaiable on
   a system and we need VRRP name generation.

   Copyright (C) 2023 EasyNetDev.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "cksum.h"

static void help(void)
{
    printf("\n");
    printf("Usage: Usage: crc32sum [OPTIONS]... [FILE]...\n\
Print or verify CRC32 checksums.\n\
\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
This tool is a stripped version of cksum CRC32 from coreutils.\n\
\n");
    printf("\n");
}

int main(int32_t argc, char **argv)
{
    FILE *fp;
    char FILE_NAME[FILENAME_MAX];

    uint_fast32_t crc;
    uintmax_t total_bytes = 0;

    int opt;

    while (1)
    {
      int option_index = 0;
      static struct option long_options[] = {
                {"help",  no_argument, 0, 'h'},
                {0, 0, 0, 0}
        };

      opt = getopt_long(argc, argv, "h",
                        long_options, &option_index);

      if (opt == -1)
        break;

      switch (opt) {
        case 'h':
          help();
          exit(EXIT_SUCCESS);
          break;

        default:
          printf("Unknown options %c.\n", opt);
          exit(EXIT_SUCCESS);
          break;
      }
    }

    if (argc > optind)
      while (argc > optind)
      {
        /* Get the filename */
        if ( strlen(argv[optind]) > FILENAME_MAX) {
          printf("Filename %s exceeded %i charactes.\n", argv[optind], FILENAME_MAX);
          optind++;
          continue;
        }

        /* Open the file in read mode */
        fp = fopen(argv[optind], "r");

        if (!fp) {
          printf("Couldn't open file %s. Error number %d.\n", FILE_NAME, errno);
          exit(EXIT_FAILURE);
        }

        if (crc_sum_stream (fp, &crc, &total_bytes))
          printf("Error on CRC32 calculation for file %s.\n", argv[optind]);

        fclose(fp);

        optind++;
      }
    else
    {
       fp = stdin;
       crc_sum_stream (fp, &crc, &total_bytes);
    }

    printf(PRIuFAST32 " " PRIuMAX "\n", crc, total_bytes);

    exit(EXIT_SUCCESS);
}
