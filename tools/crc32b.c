/*
 * cmd/crc32b.c
 * Purpose: Implement simple CRC32B checksum to be used in VRRP executor.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static uint32_t crc32b(uint8_t *message) {
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (message[i] != 0) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

static void help(void)
{
    printf("\n");
    printf("Usage: crc32b <STRING>\n");
    printf("Returns computed CRC32B of the input string.\n");
    printf("\n");
}

int main(int32_t argc, char **argv)
{
    if (argc <= 1) {
	printf("Argument must contain at least one character.\n");
	help();
	exit(EXIT_FAILURE);
    }

    //printf("0x%08x\n", crc32b((uint8_t *)argv[1]));
    printf("%u\n", crc32b((uint8_t *)argv[1]));
    exit(EXIT_SUCCESS);
}
