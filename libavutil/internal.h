/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * common internal API header
 */

#ifndef AVUTIL_INTERNAL_H
#define AVUTIL_INTERNAL_H

#if !defined(DEBUG) && !defined(NDEBUG)
#    define NDEBUG
#endif

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "config.h"
#include "attributes.h"
#include "timer.h"
#include "cpu.h"
#include "dict.h"

#ifndef attribute_align_arg
#if ARCH_X86_32 && AV_GCC_VERSION_AT_LEAST(4,2)
#    define attribute_align_arg __attribute__((force_align_arg_pointer))
#else
#    define attribute_align_arg
#endif
#endif

#if defined(_MSC_VER) && CONFIG_SHARED
#    define av_export __declspec(dllimport)
#else
#    define av_export
#endif

#ifndef INT_BIT
#    define INT_BIT (CHAR_BIT * sizeof(int))
#endif

#define FF_ALLOC_OR_GOTO(ctx, p, size, label)\
{\
    p = av_malloc(size);\
    if (p == NULL && (size) != 0) {\
        av_log(ctx, AV_LOG_ERROR, "Cannot allocate memory.\n");\
        goto label;\
    }\
}

#define FF_ALLOCZ_OR_GOTO(ctx, p, size, label)\
{\
    p = av_mallocz(size);\
    if (p == NULL && (size) != 0) {\
        av_log(ctx, AV_LOG_ERROR, "Cannot allocate memory.\n");\
        goto label;\
    }\
}

#include "libm.h"

/**
 * Return NULL if CONFIG_SMALL is true, otherwise the argument
 * without modification. Used to disable the definition of strings
 * (for example AVCodec long_names).
 */
#if CONFIG_SMALL
#   define NULL_IF_CONFIG_SMALL(x) NULL
#else
#   define NULL_IF_CONFIG_SMALL(x) x
#endif

/**
 * Define a function with only the non-default version specified.
 *
 * On systems with ELF shared libraries, all symbols exported from
 * FFmpeg libraries are tagged with the name and major version of the
 * library to which they belong.  If a function is moved from one
 * library to another, a wrapper must be retained in the original
 * location to preserve binary compatibility.
 *
 * Functions defined with this macro will never be used to resolve
 * symbols by the build-time linker.
 *
 * @param type return type of function
 * @param name name of function
 * @param args argument list of function
 * @param ver  version tag to assign function
 */
#if HAVE_SYMVER_ASM_LABEL
#   define FF_SYMVER(type, name, args, ver)                     \
    type ff_##name args __asm__ (EXTERN_PREFIX #name "@" ver);  \
    type ff_##name args
#elif HAVE_SYMVER_GNU_ASM
#   define FF_SYMVER(type, name, args, ver)                             \
    __asm__ (".symver ff_" #name "," EXTERN_PREFIX #name "@" ver);      \
    type ff_##name args;                                                \
    type ff_##name args
#endif

/**
 * Return NULL if a threading library has not been enabled.
 * Used to disable threading functions in AVCodec definitions
 * when not needed.
 */
#if HAVE_THREADS
#   define ONLY_IF_THREADS_ENABLED(x) x
#else
#   define ONLY_IF_THREADS_ENABLED(x) NULL
#endif

#if HAVE_MMX_INLINE
/**
 * Empty mmx state.
 * this must be called between any dsp function and float/double code.
 * for example sin(); dsp->idct_put(); emms_c(); cos()
 */
static av_always_inline void emms_c(void)
{
    if(av_get_cpu_flags() & AV_CPU_FLAG_MMX)
        __asm__ volatile ("emms" ::: "memory");
}
#elif HAVE_MMX && HAVE_MM_EMPTY
#   include <mmintrin.h>
#   define emms_c _mm_empty
#elif HAVE_MMX && HAVE_YASM
#   include "libavutil/x86/emms.h"
#   define emms_c avpriv_emms_yasm
#else
#   define emms_c()
#endif /* HAVE_MMX_INLINE */

#endif /* AVUTIL_INTERNAL_H */
