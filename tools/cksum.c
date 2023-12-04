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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <error.h>
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
# define BUFLEN (1 << 16)

//extern uint_fast32_t const crctab[8][256];

static int
cksum_slice8(FILE *fp, uint_fast32_t *crc_out, uintmax_t *length_out)
{
    uint32_t buf[BUFLEN / sizeof (uint32_t)];
    uint_fast32_t crc = 0;
    uintmax_t length = 0;
    size_t bytes_read;

    if (!fp || !crc_out || !length_out)
      return 0;

    while ((bytes_read = fread (buf, 1, BUFLEN, fp)) > 0)
      {
        uint32_t *datap;

        if (length + bytes_read < length)
          {
            errno = EOVERFLOW;
            return 0;
          }
        length += bytes_read;

        if (bytes_read == 0)
          {
             if (ferror (fp))
               return 0;
          }

        /* Process multiple of 8 bytes */
        datap = (uint32_t *)buf;
        while (bytes_read >= 8)
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
            bytes_read -= 8;
          }

        /* And finish up last 0-7 bytes in a byte by byte fashion */
        unsigned char *cp = (unsigned char *)datap;
        while (bytes_read--)
          crc = (crc << 8) ^ crctab[0][((crc >> 24) ^ *cp++) & 0xFF];

        if (feof(fp))
          break;
      }

    *crc_out = crc;
    *length_out = length;

    return 1;
}

/* Calculate the checksum and length in bytes of stream STREAM.
   Return -1 on error, 0 on success.  */

int
crc_sum_stream (FILE *stream, uint_fast32_t *crc_out, uintmax_t *length)
{

  uintmax_t total_bytes = 0;
  uint_fast32_t crc = 0;

  if (! cksum_slice8 (stream, &crc, &total_bytes))
    return -1;

  *length = total_bytes;

  for (; total_bytes; total_bytes >>= 8)
    crc = (crc << 8) ^ crctab[0][((crc >> 24) ^ total_bytes) & 0xFF];
  crc = ~crc & 0xFFFFFFFF;

  *crc_out = crc;

  return 0;

}
