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

/**********************************************************************/
/* implementation of fast fourier transform and transformatei         */
/* fast fourier inversion using decimation 抽取 in time algorithm.     */
/* the file contains necessary and auxiliary functions in implementing*/
/* algorithm, such as pre calculus and tabelei vectors w_n            */
/* mapping for binary order reversed.                                 */
/**********************************************************************/

#include <math.h>
#include "../common/complex.h"

/* observations on the deploy:

    for the calculation of the spectral components f (k) is a vector of complex numbers, 
    which will be initializat with functions the pelleting粒化.
    at each iteration迭代 of how it will be created the next level,
    the number of it's half and their field of definition is doubling.

   OBS 1
   -----
    it is noticed that on every level have the same total number of spectral components,
    therefore the auxiliary functions can be represented chain linked.
    an example for n = 8, to which the auxiliary functions on each level represented a step of the algorithm at:

   +-------+-------+-------+-------+-------+-------+-------+-------+
   |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |
   +===============================================================+
   | F_000 | F_001 | F_010 | F_011 | F_100 | F_101 | F_110 | F_111 |
   +-------+-------+-------+-------+-------+-------+-------+-------+
   |      F_00     |      F_01     |      F_10     |      F_11     |
   +---------------+---------------+---------------+---------------+
   |              F_0              |              F_1              |
   +-------------------------------+-------------------------------+
   |                               F                               |
   +---------------------------------------------------------------+


   OBS 2
   -----
    the functions f the granulation are defined in point 0, therefore the amount will have a single term,
     and be the unit vector w do. in other words, its value will be equal to a value in the time domain.

    moreover, the corresponding相应的 binary component reversed,
    i mean of f_011 (on the component 011(b) = 3) will correspond to the sample from the position 110(b) = 6.
     therefore the vector of values can be initializat complex with input data in binary order reversed.

   OBS 3
   -----
    the vector of values can be overridden at each iteration of how complex.

    observation vector with v. to calculate f(0) is necessary f_0(0) and f_1(0),
    so according to representation,
    v(0) will score value calculated using v(0) and v(4).
    f(4) uses all f_0(0) and f_1(0), so for v(4) using all  (0) and v(4).
    in addition, v(0) and v(4) are no longer necessary for any other value.
    result so that vector v can be overwritten calculating pairs of two components.
 */

/* 
compute a mapping table which contains, for each index, value index with binary reversed.
for example, if the size tabelei is n = 8 (3 bits),
for the component 3 = 011 (b) will correspond to the value 6 = 110 (b).
n is the number of bits of the vector map is size, and mapping.
the size of the area of memory for map must be at least 2 ^ n * sizeof (int).
 */
void bit_reverse_map(int n, int *map) {
    int i, j, max = 1 << n;
    register int b, d;

    for(j = 0; j < max; j++, map++) {
        b = j;
        d = 0;
        for(i = 0; i < n; i++) {
            d = (d << 1) | (b & 1);
            b >>= 1;
        }
        *map = d;
    }
}

/* compute the scoreboard with vectors w, where w_n ^ k = exp(2 * pi * k / n).
n is the number of bits of the size, and vector map is complex values which will be generated.
the size of the area of memory for map must be at least 2^n * sizeof (t_complex).  */
void w_map(int n, t_complex *map) {
    int i, max = 1 << n;
    double N = max;
    double p = (double)((int)1 << COMPLEX_PRECISION);
    double arg;

    for(i = 0; i < max; i++) {
        arg = 2.0 * M_PI * i / N;
        map[i].r = p * cos(arg);
        map[i].i = p * sin(arg);
    }
}

/* copying data from a binary vector to another, in reverse order.
n is the number of bits of the map size and map is a binary index order reversed
(possibly generated by bit_reverse_map).
 */
void bit_reverse(int n, int *map, t_complex *src, t_complex *dst) {
    int i, max = 1 << n;
    for (i = 0; i < max; i++, src++, map++)
        dst[*map] = *src;
}

/* binary data must be reversed in order. */
void dec_time_fft(int n, t_complex *w, t_complex *data) {
    int max = 1 << n;
    int size = 1;
    int mul_k = 1 << (n - 1);
    int w0_exp, w1_exp, w1_exp_init = mul_k;
    int i, j, k;

    /* mask will not have the bits set and will be used for the calculation of the quartering四分法 and fast 2^n, 
    necessary normalizarii index in the powers of w_n (powers W_n are periodic, period 2^n)
     */
    int w_exp_mask = max - 1;
    t_complex q0, q1, w_pow;

    for(i = 0; i < n; i++) {
        /* for the "recursivitate复发性的" loop*/
        for(j = 0; j < max; j += size << 1) {
            /* loop for functions f_xx on current level;
            every step is the calculation of a single function f_xx on current level
             */
            w0_exp = 0;
            /* w1_exp = k_mul * size; */ /* the product is constant */
            w1_exp = w1_exp_init;

            for(k = 0; k < size; k++) {
                /* the index for the component of the function f_xx current coincides重合 with the notation of the documentation */

                /* calculation for w_n exponent value and extraction for power w_n of the precalculata.
                the value is actually negative, so make a correction after calculating the quartering.
                 */
                w_pow = w[max - (w0_exp & w_exp_mask ? : max)];

                q0.r = data[j + k].r + cmul_r(data[j + size + k], w_pow);
                q0.i = data[j + k].i + cmul_i(data[j + size + k], w_pow);

                w_pow = w[max - (w1_exp & w_exp_mask ? : max)];

                q1.r = data[j + k].r + cmul_r(data[j + size + k], w_pow);
                q1.i = data[j + k].i + cmul_i(data[j + size + k], w_pow);

                data[j + k] = q0;
                data[j + size + k] = q1;

                /* update exponenti w */
                w0_exp += mul_k;
                w1_exp += mul_k;
            }
        }
        size <<= 1;
        mul_k >>= 1;
    }
}

/* compute the discrete fourier transform algorithm reverse using decimation in time.

   binary data must be reversed in order.

   considering the symmetry relations, this algorithm is almost identical to that for direct transformation.
   the only differences are the following:
   - is no longer necessary for correction of sign vectors exponentii w_n.
   - there is a factor of 1 / 2 ^ n, which is distributed in many a factor 1 / 2 for each iteration of how
 */
void dec_time_ifft(int n, t_complex *w, t_complex *data) {
    int max = 1 << n;
    int size = 1;
    int mul_k = 1 << (n - 1);
    int w0_exp, w1_exp, w1_exp_init = mul_k;
    int i, j, k;

    int w_exp_mask = max - 1;
    t_complex q0, q1, w_pow;

    for(i = 0; i < n; i++) {
        /* for the "recursivitate" loop */
        for(j = 0; j < max; j += size << 1) {
            /* loop for functions f_xx on current level;
            every step is the calculation of a single function f_xx on current level
             */
            w0_exp = 0;
            /* w1_exp = k_mul * size; */ /* the product is constant */
            w1_exp = w1_exp_init;

            for(k = 0; k < size; k++) {
                /* the index for the component of the function f_xx current coincides with the notation of the documentation; */
                w_pow = w[w0_exp & w_exp_mask];

                q0.r = (data[j + k].r + cmul_r(data[j + size + k], w_pow)) / 2;
                q0.i = (data[j + k].i + cmul_i(data[j + size + k], w_pow)) / 2;

                w_pow = w[w1_exp & w_exp_mask];

                q1.r = (data[j + k].r + cmul_r(data[j + size + k], w_pow)) / 2;
                q1.i = (data[j + k].i + cmul_i(data[j + size + k], w_pow)) / 2;

                data[j + k] = q0;
                data[j + size + k] = q1;

                /* update exponenti w */
                w0_exp += mul_k;
                w1_exp += mul_k;
            }
        }
        size <<= 1;
        mul_k >>= 1;
    }
}
