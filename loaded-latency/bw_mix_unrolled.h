/*
 * SPDX-FileCopyrightText: Copyright 2019-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef BW_MIX_UNROLLED_H
#define BW_MIX_UNROLLED_H

#include "bandwidth.h"

void my_mix_100r_0w(struct bw_thread_info const *bw_tinfo);
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
void my_mix_25r_75w(struct bw_thread_info const *bw_tinfo);
void my_mix_0r_100w(struct bw_thread_info const *bw_tinfo);

#endif /* BW_MIX_UNROLLED_H */
