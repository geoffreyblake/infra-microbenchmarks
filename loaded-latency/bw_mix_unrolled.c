/*
 * SPDX-FileCopyrightText: Copyright 2019-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <stdint.h>
#include "bandwidth.h"

/* Macros to generate unrolled load/store sequences */
#ifdef __aarch64__
#define LOAD_OP(dummy, p, offset) \
    asm volatile ("ldr %0, [%1, %2]" : "=r" (dummy) : "r" (p), "r" (offset))
#define STORE_OP(dummy, p, offset) \
    asm volatile ("str %0, [%1, %2]" : : "r" (dummy), "r" (p), "r" (offset) : "memory")
#endif

#ifdef __x86_64__
#define LOAD_OP(dummy, p, offset) \
    asm volatile ("movq (%1,%2,1), %0" : "=r" (dummy) : "r" (p), "r" (offset))
#define STORE_OP(dummy, p, offset) \
    asm volatile ("movq %0, (%1,%2,1)" : : "r" (dummy), "r" (p), "r" (offset) : "memory")
#endif

#define LOAD_INCR(dummy, p, offset, stride) \
    do { LOAD_OP(dummy, p, offset); offset += stride; } while(0)
#define STORE_INCR(dummy, p, offset, stride) \
    do { STORE_OP(dummy, p, offset); offset += stride; } while(0)

/* Generate N loads */
#define LOADS_20(dummy, p, offset, stride) \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_OP(dummy, p, offset)

#define LOADS_18(dummy, p, offset, stride) \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride)

#define LOADS_16(dummy, p, offset, stride) \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride)

#define LOADS_14(dummy, p, offset, stride) \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride)

#define LOADS_12(dummy, p, offset, stride) \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride)

#define LOADS_10(dummy, p, offset, stride) \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride); \
    LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride)

/* Generate N stores */
#define STORES_10(dummy, p, offset, stride) \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_OP(dummy, p, offset)

#define STORES_8(dummy, p, offset, stride) \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride)

#define STORES_6(dummy, p, offset, stride) \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride)

#define STORES_4(dummy, p, offset, stride) \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride); \
    STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride)

#define STORES_2(dummy, p, offset, stride) \
    STORE_INCR(dummy, p, offset, stride); STORE_OP(dummy, p, offset)

/* 100% reads, 0% writes - 20 reads */
void my_mix_100r_0w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOADS_20(dummy, p, offset, stride);
        delay_loop(inner_nops);
    }
}

/* 90% reads, 10% writes - 18 reads + 2 writes */
void my_mix_90r_10w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOADS_18(dummy, p, offset, stride);
        offset += stride;
        STORES_2(dummy, p, offset, stride);
        delay_loop(inner_nops);
    }
}

/* 80% reads, 20% writes - 16 reads + 4 writes */
void my_mix_80r_20w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOADS_16(dummy, p, offset, stride);
        offset += stride;
        STORES_4(dummy, p, offset, stride);
        delay_loop(inner_nops);
    }
}

/* 70% reads, 30% writes - 14 reads + 6 writes */
void my_mix_70r_30w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOADS_14(dummy, p, offset, stride);
        offset += stride;
        STORES_6(dummy, p, offset, stride);
        delay_loop(inner_nops);
    }
}

/* 60% reads, 40% writes - 12 reads + 8 writes */
void my_mix_60r_40w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOADS_12(dummy, p, offset, stride);
        offset += stride;
        STORES_8(dummy, p, offset, stride);
        delay_loop(inner_nops);
    }
}

/* 50% reads, 50% writes - 10 reads + 10 writes */
void my_mix_50r_50w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOADS_10(dummy, p, offset, stride);
        offset += stride;
        STORES_10(dummy, p, offset, stride);
        delay_loop(inner_nops);
    }
}

/* 95% reads, 5% writes - 19 reads + 1 write (kept at 20 ops) */
void my_mix_95r_5w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); STORE_OP(dummy, p, offset);
        delay_loop(inner_nops);
    }
}

/* 85% reads, 15% writes - 17 reads + 3 writes (kept at 20 ops) */
void my_mix_85r_15w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_OP(dummy, p, offset);
        delay_loop(inner_nops);
    }
}

/* 75% reads, 25% writes - 15 reads + 5 writes */
void my_mix_75r_25w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_OP(dummy, p, offset);
        delay_loop(inner_nops);
    }
}

/* 65% reads, 35% writes - 13 reads + 7 writes */
void my_mix_65r_35w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_OP(dummy, p, offset);
        delay_loop(inner_nops);
    }
}

/* 55% reads, 45% writes - 11 reads + 9 writes */
void my_mix_55r_45w(struct bw_thread_info const *bw_tinfo) {
    void *p = bw_tinfo->mem;
    size_t bytes = bw_tinfo->bw_buflen;
    size_t stride = bw_tinfo->bw_stride;
    size_t random_jump_freq = bw_tinfo->bw_random_jump_freq;
    uint32_t rng_state = (uint32_t)(uintptr_t)p;
    size_t inner_nops = bw_tinfo->inner_nops;
    size_t dummy = 0;
    size_t op_count = 0;
    size_t num_strides = bytes / stride;

    for (size_t i = 0; i < bytes; i += stride * 20) {
        size_t offset = get_offset(i, random_jump_freq, op_count++, &rng_state, num_strides, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride); LOAD_INCR(dummy, p, offset, stride);
        LOAD_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_INCR(dummy, p, offset, stride); STORE_INCR(dummy, p, offset, stride);
        STORE_OP(dummy, p, offset);
        delay_loop(inner_nops);
    }
}
