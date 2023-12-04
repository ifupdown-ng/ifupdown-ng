#ifndef __CKSUM_H
#define __CKSUM_H

extern int
crc_sum_stream (FILE *stream, uint_fast32_t *crc_out, uintmax_t *length);

/*
extern int
cksum_pclmul (FILE *fp, uint_fast32_t *crc_out, uintmax_t *length_out);
*/

extern uint_fast32_t const crctab[8][256];

#endif /* __CKSUM_H */
