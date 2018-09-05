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

#ifndef _WAVE_H
#define _WAVE_H

#include "../common/types.h"
#include "../common/complex.h"

/* FIXME next are valid only i386 */

/* get a t_s16 of contents from the address of p */
#define WAVE_s16_2_MACHINE(p) (*(t_s16 *)(p))

/* submit a t_s16 to the address of p */
#define WAVE_MACHINE_2_s16(p,x) (*(t_s16 *)(p) = (x))

/* structures for WAV file format */

#ifdef __GNUC__
#define ATTRIBUTE_PACKED __attribute((packed))
#else
#define ATTRIBUTE_PACKED
#endif

typedef struct _chunk {
    char id[4];
    t_u32 size;
} ATTRIBUTE_PACKED t_chunk;

typedef struct _riff_hdr {
    t_chunk chunk;
    char file_type[4];
} ATTRIBUTE_PACKED t_riff_hdr;

typedef struct _wave_format {
    t_s16 format_tag;
    t_s16 channels;
    t_s32 samples_per_sec;
    t_s32 bytes_per_sec;
    t_s16 block_align;
} ATTRIBUTE_PACKED t_wave_format;

typedef struct _wave_format_ex {
    t_s16 format_tag;
    t_s16 channels;
    t_s32 samples_per_sec;
    t_s32 bytes_per_sec;
    t_s16 block_align;
    t_s16 bits_per_sample;
    /* t_s16 cb_size; */
} ATTRIBUTE_PACKED t_wave_format_ex;

#define WAVE_FORMAT_PCM 1

typedef void (*t_complex_promote)(int, int, char *, t_complex *);
typedef void (*t_complex_reduce)(int, int, t_complex *, char *);

extern void complex_promote_s16(int, int, char *, t_complex *);
extern void complex_reduce_s16(int, int, t_complex *, char *);
extern void complex_promote_u8(int, int, char *, t_complex *);
extern void complex_reduce_u8(int, int, t_complex *, char *);

extern void weight_map(int, t_complex *);
#endif
