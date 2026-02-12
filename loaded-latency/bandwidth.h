
/*
 * SPDX-FileCopyrightText: Copyright 2019-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef BANDWIDTH_H
#define BANDWIDTH_H

#include <stdint.h>
#include <pthread.h>

enum bw_op_e {
    BW_OP_READ = 0,
    BW_OP_MEMCPY,
    BW_OP_MEMCPY_NOINOPS,
    BW_OP_WRITE,
    BW_OP_MEMSET1,
    BW_OP_MEMSET64,
    BW_OP_MIX_100R,
    BW_OP_MIX_5W,
    BW_OP_MIX_10W,
    BW_OP_MIX_15W,
    BW_OP_MIX_20W,
    BW_OP_MIX_25W,
    BW_OP_MIX_30W,
    BW_OP_MIX_35W,
    BW_OP_MIX_40W,
    BW_OP_MIX_45W,
    BW_OP_MIX_50W,
    NUM_BW_OPS
};

struct bw_thread_info {
    pthread_t     thread_id;
    unsigned long hwcounter_start;
    unsigned long hwcounter_stop;
    unsigned long actual_hwcounter_start;   // output
    unsigned long actual_hwcounter_stop;    // output
    int           thread_num;
    int           cpu;              // cpu on which this thread is run
    size_t        bw_buflen;        // bytes
    size_t        inner_nops;
    size_t        outer_nops;
    size_t        iterations;
    size_t        bw_cacheline_bytes;
    int           bw_use_hugepages;
    enum bw_op_e  bw_op;
    size_t        bw_stride;        // stride in bytes
    size_t        bw_random_jump_freq; // jump to random location every N iterations
    double        avg_bw;                   // output
    unsigned long long bw_bytes_read;       // output
    unsigned long long bw_bytes_written;    // output
    unsigned long long bw_bytes_rw;         // output
    void *        mem;              // memory buffer
    char          threadname[32];
};

/* Simple LCG pseudo-random number generator */
static inline uint32_t prng(uint32_t *state) {
    *state = (*state * 1103515245u + 12345u) & 0x7fffffffu;
    return *state;
}

/* Helper to calculate offset with optional random jumping */
static inline size_t get_offset(size_t i, size_t random_jump_freq, size_t op_count,
                                uint32_t *rng_state, size_t num_strides, size_t stride) {
    if (random_jump_freq && (op_count % random_jump_freq == 0)) {
        return (prng(rng_state) % num_strides) * stride;
    }
    return i;
}

/* Helper to do a busy wait loop for some cycles instead of doing the iteration immediately */
static inline void delay_loop(size_t inner_nops)
{
    for (size_t j = 0; j < inner_nops; j++) asm volatile ("");
}

void bandwidth_thread (struct bw_thread_info * bw_tinfo);

/* Unrolled mixed read/write patterns */
void my_mix_95r_5w(struct bw_thread_info const *bw_tinfo);
void my_mix_90r_10w(struct bw_thread_info const *bw_tinfo);
void my_mix_85r_15w(struct bw_thread_info const *bw_tinfo);
void my_mix_80r_20w(struct bw_thread_info const *bw_tinfo);
void my_mix_75r_25w(struct bw_thread_info const *bw_tinfo);
void my_mix_70r_30w(struct bw_thread_info const *bw_tinfo);
void my_mix_65r_35w(struct bw_thread_info const *bw_tinfo);
void my_mix_60r_40w(struct bw_thread_info const *bw_tinfo);
void my_mix_55r_45w(struct bw_thread_info const *bw_tinfo);
void my_mix_50r_50w(struct bw_thread_info const *bw_tinfo);

#endif
