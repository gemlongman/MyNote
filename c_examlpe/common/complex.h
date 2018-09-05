/**********************************************************************/
/*  definition of types and arithmetic on complex numbers            */
/**********************************************************************/

#ifndef _COMPLEX_H
#define _COMPLEX_H

#include "types.h"

typedef struct t_complex {
    t_s64       r;
    t_s64       i;
} t_complex;

/* the number of bits (lower) used to represent the components of a complex number subunitara */
#define COMPLEX_PRECISION 16

/* the product of two complex numbers real the real product of two complex numbers 两个复数的实积 */
#define cmul_r(x,y) (((x).r * (y).r - (x).i * (y).i) / (1 << COMPLEX_PRECISION))

/* imaginary component product of two numbers 虚部积 */
#define cmul_i(x,y) (((x).r * (y).i + (x).i * (y).r) / (1 << COMPLEX_PRECISION))

/* module of a complex number */
#define cmodulus(x) (((x).r * (x).r + (x).i * (x).i) >> COMPLEX_PRECISION)

#endif
