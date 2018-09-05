/*****************************************************************************
 * wavetrans - a simple wave transformation tool                             *
 * Copyright (C) 2005 Radu Rendec <rrendec@yahoo.com>                        *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * Foobar is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with Foobar; if not, write to the Free Software                     *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *                                                                           *
 *****************************************************************************/

#include <math.h>

#include "wave.h"

#define RANGE(x,min,max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

/* extract samples from a vector data (obtained for example from a wave file) and file them in a vector of complex numbers..
n represents a number of samples (and not the number of bits of size), 
to be able to process and the last window of the file, which is probably not complete and will be filled with 0.
 */
void complex_promote_s16(int n, int interleave, char *src, t_complex *dst) {
    int i;

    for(i = 0; i < n; i++, src += interleave, dst++) {
        dst->r = (t_s64)WAVE_s16_2_MACHINE(src) * (1 << COMPLEX_PRECISION);
        dst->i = 0;
    }
}

void complex_reduce_s16(int n, int interleave, t_complex *src, char *dst) {
    int i;
    t_s64 reduced;

    for(i = 0; i < n; i++, src++, dst += interleave) {
        reduced = src->r / (1 << COMPLEX_PRECISION);
        /* FIXME: rounding up the bits of precision when */
        WAVE_MACHINE_2_s16(dst, (t_s16)RANGE(reduced, -32768, 32767));
    }
}

void complex_promote_u8(int n, int interleave, char *src, t_complex *dst) {
    int i;

    for(i = 0; i < n; i++, src += interleave, dst++) {
        dst->r = ((long)(*(unsigned char *)src) - 128) * (1 << COMPLEX_PRECISION);
        dst->i = 0;
    }
}

void complex_reduce_u8(int n, int interleave, t_complex *src, char *dst) {
    int i;
    t_s64 reduced;

    for(i = 0; i < n; i++, src++, dst += interleave) {
        reduced = src->r / (1 << COMPLEX_PRECISION) + 128;
        /* FIXME: rounding up the bits of precision when */
        *(unsigned char *)dst = (unsigned char)RANGE(reduced, 0, 255);
    }
}

void weight_map(int n, t_complex *data) {
    int i, max = 1 << n;

    double precision = 1 << COMPLEX_PRECISION;

    for(i = 0; i < max; i++, data++) {
        data->r = precision * (1 - cos(2.0 * M_PI * i / max)) / 2;
        data->i = 0;
    }
}
