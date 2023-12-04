/*
 * cmd/cksum.c
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cksum.h"

#include <byteswap.h>
#ifdef WORDS_BIGENDIAN
# define SWAP(n) (n)
#else
# define SWAP(n) bswap_32 (n)
#endif

/* Number of bytes to read at once.  */
# define BUFLEN (1 << 5)

static int
cksum_slice8(const char *iface, uint_fast32_t *crc_out, uintmax_t *length_out)
{

    uint_fast32_t crc = 0;
    uintmax_t length = 0;
    uint32_t *datap;
    size_t str_len = strlen(iface);

    if (!iface || !crc_out || !length_out)
      return 0;

    length = str_len;

    /* Process multiple of 8 bytes */
    datap = (uint32_t *)iface;

    while (str_len >= 8)
    {

	uint32_t first = *datap++, second = *datap++;
	crc ^= SWAP (first);
	second = SWAP (second);
	crc = (crctab[7][(crc >> 24) & 0xFF]
	       ^ crctab[6][(crc >> 16) & 0xFF]
	       ^ crctab[5][(crc >>  8) & 0xFF]
	       ^ crctab[4][ crc        & 0xFF]
	       ^ crctab[3][(second >> 24) & 0xFF]
	       ^ crctab[2][(second >> 16) & 0xFF]
	       ^ crctab[1][(second >>  8) & 0xFF]
	       ^ crctab[0][ second        & 0xFF]);

	str_len -= 8;
    }

    /* And finish up last 0-7 bytes in a byte by byte fashion */
    unsigned char *cp = (unsigned char *)datap;
    while (str_len--)
        crc = (crc << 8) ^ crctab[0][((crc >> 24) ^ *cp++) & 0xFF];

    *crc_out = crc;
    *length_out = length;

    return 1;
}

/* Calculate the checksum and length in bytes of stream STREAM.
   Return -1 on error, 0 on success.  */

int
crc_sum_string (const char *iface, uint_fast32_t *crc_out, uintmax_t *length)
{

  uintmax_t total_bytes = 0;
  uint_fast32_t crc = 0;

  if (! cksum_slice8 (iface, &crc, &total_bytes))
    return -1;

  *length = total_bytes;

  for (; total_bytes; total_bytes >>= 8)
    crc = (crc << 8) ^ crctab[0][((crc >> 24) ^ total_bytes) & 0xFF];
  crc = ~crc & 0xFFFFFFFF;

  *crc_out = crc;

  return 0;

}
