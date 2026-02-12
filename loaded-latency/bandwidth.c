
/*
 * SPDX-FileCopyrightText: Copyright 2019-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>

#ifdef USE_HUGEPAGES
#include <sys/mman.h>
#endif

#ifdef __aarch64__
#include "cntvct.h"
#endif

#ifdef __x86_64__
#include "rdtsc.h"
#endif

#include "alloc.h"
#include "bandwidth.h"
#include "bw_mix_unrolled.h"

#define MEMSET_PATTERN_BYTE     0xA5
#define MEMSET_PATTERN_QWORD    0xA5A5A5A5A5A5A5A5ULL
#define CACHE_LINE_SIZE_64      64

#ifndef STREAM_TYPE
#define STREAM_TYPE             double
#endif

typedef void (*bw_op_func_t)(struct bw_thread_info const *bw_tinfo);

static void my_read(struct bw_thread_info const *bw_tinfo) __attribute__((noinline));
static void my_memcpy(struct bw_thread_info const *bw_tinfo) __attribute__((noinline));
static void my_memcpy_no_inner_nops(struct bw_thread_info const *bw_tinfo) __attribute__((noinline));
static void my_write(struct bw_thread_info const *bw_tinfo) __attribute__((noinline));
static void my_memset1(struct bw_thread_info const *bw_tinfo) __attribute__((noinline));
static void my_memset64(struct bw_thread_info const *bw_tinfo) __attribute__((noinline));


static void my_mix_100r(struct bw_thread_info const *bw_tinfo) { my_mix_100r_0w(bw_tinfo); }
static void my_mix_5w(struct bw_thread_info const *bw_tinfo) { my_mix_95r_5w(bw_tinfo); }
static void my_mix_10w(struct bw_thread_info const *bw_tinfo) { my_mix_90r_10w(bw_tinfo); }
static void my_mix_15w(struct bw_thread_info const *bw_tinfo) { my_mix_85r_15w(bw_tinfo); }
static void my_mix_20w(struct bw_thread_info const *bw_tinfo) { my_mix_80r_20w(bw_tinfo); }
static void my_mix_25w(struct bw_thread_info const *bw_tinfo) { my_mix_75r_25w(bw_tinfo); }
static void my_mix_30w(struct bw_thread_info const *bw_tinfo) { my_mix_70r_30w(bw_tinfo); }
static void my_mix_35w(struct bw_thread_info const *bw_tinfo) { my_mix_65r_35w(bw_tinfo); }
static void my_mix_40w(struct bw_thread_info const *bw_tinfo) { my_mix_60r_40w(bw_tinfo); }
static void my_mix_45w(struct bw_thread_info const *bw_tinfo) { my_mix_55r_45w(bw_tinfo); }
static void my_mix_50w(struct bw_thread_info const *bw_tinfo) { my_mix_50r_50w(bw_tinfo); }


static const bw_op_func_t bw_op_funcs[NUM_BW_OPS] = {
    &my_read,
    &my_memcpy,
    &my_memcpy_no_inner_nops,
    &my_write,
    &my_memset1,
    &my_memset64,
    &my_mix_100r,
    &my_mix_5w,
    &my_mix_10w,
    &my_mix_15w,
    &my_mix_20w,
    &my_mix_25w,
    &my_mix_30w,
    &my_mix_35w,
    &my_mix_40w,
    &my_mix_45w,
    &my_mix_50w,
};


/* my_read() provides a variable read bandwidth.
   Increasing inner_nops lowers the read bandwidth. */
static void my_read(struct bw_thread_info const *bw_tinfo) {
    size_t i, dummy;
    void * p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t bw_cacheline_bytes = bw_tinfo->bw_cacheline_bytes;

    // this just reads one 64-bit dword from each cache line
    for (i = 0; i < bytes; i += bw_cacheline_bytes) {
#ifdef __aarch64__
        asm volatile ("ldr %0, [%1, %2]" : "=r" (dummy): "r" (p), "r" (i));
#endif
#ifdef __x86_64__
        asm volatile ("movq   (%1,%2,1), %0" : "=r" (dummy) : "r" (p), "r" (i));
#endif
        delay_loop(inner_nops);
    }
}

static void my_memcpy(struct bw_thread_info const *bw_tinfo) {
    size_t i;
    size_t dummy0, dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7;
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    void *d = (void *)((uintptr_t)p + (bytes / 2));
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t bw_cacheline_bytes = bw_tinfo->bw_cacheline_bytes;

    for (i = 0; i < (bytes / 2); i += bw_cacheline_bytes) {

        #ifdef __aarch64__
        asm volatile (
            "ldp %0, %1, [%8], #0x10\n"
            "ldp %2, %3, [%8], #0x10\n"
            "ldp %4, %5, [%8], #0x10\n"
            "ldp %6, %7, [%8], #0x10\n"
            "stp %0, %1, [%9], #0x10\n"
            "stp %2, %3, [%9], #0x10\n"
            "stp %4, %5, [%9], #0x10\n"
            "stp %6, %7, [%9], #0x10\n"
            : "=r" (dummy0), "=r" (dummy1), "=r" (dummy2), "=r" (dummy3),
              "=r" (dummy4), "=r" (dummy5), "=r" (dummy6), "=r" (dummy7),
              "+r" (p), "+r" (d)
            : "r" (p), "r" (d)
        );
        #endif
        #ifdef __x86_64__
        memcpy(d + i, p + i, bw_cacheline_bytes);
        #endif

        delay_loop(inner_nops);
    }
}

static void my_memcpy_no_inner_nops(struct bw_thread_info const *bw_tinfo) {
    size_t i;
    size_t dummy0, dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7;
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    void *d = (void *)((uintptr_t)p + (bytes / 2));
    size_t bw_cacheline_bytes = bw_tinfo->bw_cacheline_bytes;

    for (i = 0; i < (bytes / 2); i += bw_cacheline_bytes) {
#ifdef __aarch64__
        asm volatile (
            "ldp %0, %1, [%8], #0x10\n"
            "ldp %2, %3, [%8], #0x10\n"
            "ldp %4, %5, [%8], #0x10\n"
            "ldp %6, %7, [%8], #0x10\n"
            "stp %0, %1, [%9], #0x10\n"
            "stp %2, %3, [%9], #0x10\n"
            "stp %4, %5, [%9], #0x10\n"
            "stp %6, %7, [%9], #0x10\n"
            : "=r" (dummy0), "=r" (dummy1), "=r" (dummy2), "=r" (dummy3),
              "=r" (dummy4), "=r" (dummy5), "=r" (dummy6), "=r" (dummy7),
              "+r" (p), "+r" (d)
            : "r" (p), "r" (d)
        );
#endif

    }
#ifdef __x86_64__
    size_t copy_bytes = bytes / 2;
    memcpy(d, p, copy_bytes);
#endif
}


/* my_write() provides a variable write bandwidth.
   Increasing inner_nops lowers the write bandwidth. */

static void my_write(struct bw_thread_info const *bw_tinfo) {
    size_t i;
    STREAM_TYPE *p = (STREAM_TYPE *)bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t num_elems = bytes / sizeof(STREAM_TYPE);
    STREAM_TYPE scalar = (STREAM_TYPE)MEMSET_PATTERN_BYTE;

    for (i = 0; i < num_elems; i++) {
        p[i] = scalar;
        delay_loop(inner_nops);
    }
}

static void my_memset1(struct bw_thread_info const *bw_tinfo) {
    size_t i;
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t bw_cacheline_bytes = bw_tinfo->bw_cacheline_bytes;

    for (i = 0; i < bytes; i += bw_cacheline_bytes) {
#ifdef __aarch64__
        asm volatile ("strb %w0, [%1, %2]" : : "r" (MEMSET_PATTERN_BYTE), "r" (p), "r" (i) : "memory");
#endif
#ifdef __x86_64__
        asm volatile ("movb %b0, (%1,%2,1)" : : "r" ((char)MEMSET_PATTERN_BYTE), "r" (p), "r" (i) : "memory");
#endif
        delay_loop(inner_nops);
    }
}

static void my_memset64(struct bw_thread_info const *bw_tinfo) {
    size_t i;
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t bw_cacheline_bytes = bw_tinfo->bw_cacheline_bytes;
    uint64_t pattern = MEMSET_PATTERN_QWORD;

    for (i = 0; i < bytes; i += bw_cacheline_bytes) {
        void *cache_line = (void *)((uintptr_t)p + i);
#ifdef __aarch64__
        asm volatile (
            "stp %1, %2, [%0], #0x10\n"
            "stp %3, %4, [%0], #0x10\n"
            "stp %5, %6, [%0], #0x10\n"
            "stp %7, %8, [%0], #0x10\n"
            : "+r" (cache_line)
            : "r" (MEMSET_PATTERN_QWORD), "r" (MEMSET_PATTERN_QWORD),
              "r" (MEMSET_PATTERN_QWORD), "r" (MEMSET_PATTERN_QWORD),
              "r" (MEMSET_PATTERN_QWORD), "r" (MEMSET_PATTERN_QWORD),
              "r" (MEMSET_PATTERN_QWORD), "r" (MEMSET_PATTERN_QWORD)
            : "memory"
        );
#endif
#ifdef __x86_64__
        memset(cache_line, MEMSET_PATTERN_BYTE, bw_cacheline_bytes);
#endif
        delay_loop(inner_nops);
    }
}


void bandwidth_thread (struct bw_thread_info * bw_tinfo) {
    size_t buflen           = bw_tinfo->bw_buflen;
    size_t inner_nops       = bw_tinfo->inner_nops;
    size_t outer_nops       = bw_tinfo->outer_nops;

    size_t iterations       = bw_tinfo->iterations;
    int thread_num          = bw_tinfo->thread_num;
    int cpu                 = bw_tinfo->cpu;                /* cpu on which this thread is to run */

    unsigned long hwcounter_start = bw_tinfo->hwcounter_start;
    unsigned long hwcounter_stop  = bw_tinfo->hwcounter_stop;

    size_t bw_cacheline_bytes     = bw_tinfo->bw_cacheline_bytes;

    int bw_use_hugepages    = bw_tinfo->bw_use_hugepages;

    unsigned long start_tick, stop_tick, tickdiff;
    double avg_bw = 0.0;
    double cntfreq = (double) read_cntfreq();
    unsigned long bw_samples = 0;
    bw_op_func_t bw_op;

    unsigned long long bw_total_bytes_read = 0;
    unsigned long long bw_total_bytes_written = 0;

    bw_op = bw_op_funcs[bw_tinfo->bw_op];

    printf("CPU%d BWTHREAD%d: buflen = %zu, iterations = %zu, inner_nops = %zu, outer_nops = %zu, hwcounter_start = 0x%zx, bw_cacheline_bytes = %zu, bw_use_hugepages = %d\n",
           cpu, thread_num, buflen, iterations, inner_nops, outer_nops, hwcounter_start, bw_cacheline_bytes, bw_use_hugepages);

    void * mem = do_alloc(buflen, bw_use_hugepages, sysconf(_SC_PAGESIZE));
    memset(mem, 0xA, buflen);
    bw_tinfo->mem = mem;

    // synchronize thread start at the specified HW timer value
    while ((start_tick = read_hwcounter()) < hwcounter_start) {
        ;
    }

    bw_tinfo->actual_hwcounter_start = start_tick;

    printf("CPU%d BWTHREAD%d: started at " HWCOUNTER " = 0x%zx\n", cpu, thread_num, start_tick);

    while ((start_tick = stop_tick = read_hwcounter()) < hwcounter_stop) {
        double bw;
        unsigned long long sample_bw_bytes_read = 0;
        unsigned long long sample_bw_bytes_written = 0;

        for (size_t i = 0; i < iterations; i++) {
            bw_op(bw_tinfo);
            for (size_t j = 0; j < outer_nops; j++) {
                asm volatile ("");
            }
        }

        stop_tick = read_hwcounter();
        tickdiff = stop_tick - start_tick;

        bw = iterations * buflen / (tickdiff / cntfreq);

        switch (bw_tinfo->bw_op) {
            case BW_OP_READ:
                sample_bw_bytes_read += buflen;
                break;
            case BW_OP_MEMCPY:
            case BW_OP_MEMCPY_NOINOPS:
                sample_bw_bytes_read += buflen / 2;
                sample_bw_bytes_written += buflen / 2;
                break;
            case BW_OP_MEMSET1:
                sample_bw_bytes_written += buflen / bw_tinfo->bw_cacheline_bytes;
                break;
            case BW_OP_WRITE:
            case BW_OP_MEMSET64:
                sample_bw_bytes_written += buflen;
                break;
            case BW_OP_MIX_100R:
                sample_bw_bytes_read += buflen;
                break;
            case BW_OP_MIX_5W:
                sample_bw_bytes_read += buflen * 95 / 100;
                sample_bw_bytes_written += buflen * 5 / 100;
                break;
            case BW_OP_MIX_10W:
                sample_bw_bytes_read += buflen * 90 / 100;
                sample_bw_bytes_written += buflen * 10 / 100;
                break;
            case BW_OP_MIX_15W:
                sample_bw_bytes_read += buflen * 85 / 100;
                sample_bw_bytes_written += buflen * 15 / 100;
                break;
            case BW_OP_MIX_20W:
                sample_bw_bytes_read += buflen * 80 / 100;
                sample_bw_bytes_written += buflen * 20 / 100;
                break;
            case BW_OP_MIX_25W:
                sample_bw_bytes_read += buflen * 75 / 100;
                sample_bw_bytes_written += buflen * 25 / 100;
                break;
            case BW_OP_MIX_30W:
                sample_bw_bytes_read += buflen * 70 / 100;
                sample_bw_bytes_written += buflen * 30 / 100;
                break;
            case BW_OP_MIX_35W:
                sample_bw_bytes_read += buflen * 65 / 100;
                sample_bw_bytes_written += buflen * 35 / 100;
                break;
            case BW_OP_MIX_40W:
                sample_bw_bytes_read += buflen * 60 / 100;
                sample_bw_bytes_written += buflen * 40 / 100;
                break;
            case BW_OP_MIX_45W:
                sample_bw_bytes_read += buflen * 55 / 100;
                sample_bw_bytes_written += buflen * 45 / 100;
                break;
            case BW_OP_MIX_50W:
                sample_bw_bytes_read += buflen * 50 / 100;
                sample_bw_bytes_written += buflen * 50 / 100;
                break;
            default:
                break;
        }

        bw_total_bytes_read += iterations * sample_bw_bytes_read;
        bw_total_bytes_written += iterations * sample_bw_bytes_written;

        avg_bw += bw;
        bw_samples++;

        bw /= 1e6;  // MB, not MiB

        printf("CPU%d BWTHREAD%d: %f MB/sec\n", cpu, thread_num, bw);

    }

    bw_tinfo->actual_hwcounter_stop = stop_tick;

    avg_bw /= bw_samples;

    bw_tinfo->avg_bw = avg_bw;
    bw_tinfo->bw_bytes_read = bw_total_bytes_read;
    bw_tinfo->bw_bytes_written = bw_total_bytes_written;
    bw_tinfo->bw_bytes_rw = bw_total_bytes_read + bw_total_bytes_written;
}
