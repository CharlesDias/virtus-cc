/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>

#define THREAD0_STACKSIZE 512
#define THREAD1_STACKSIZE 512

#define THREAD0_PRIORITY 2
#define THREAD1_PRIORITY 3

static inline void emulate_work()
{
    for (volatile int count_out = 0; count_out < 104000; count_out++);
}

void thread0(void)
{
    uint64_t time_stamp;
    int64_t delta_time;

    while (1)
    {
        time_stamp = k_uptime_get();
        emulate_work();
        delta_time = k_uptime_delta(&time_stamp);

        printk("thread0 yielding this round in %lld ms\n", delta_time);
        k_msleep(20);
    }
}

void thread1(void)
{
    uint64_t time_stamp;
    int64_t delta_time;

    while (1)
    {
        time_stamp = k_uptime_get();
        emulate_work();
        delta_time = k_uptime_delta(&time_stamp);

        printk("thread1 yielding this round in %lld ms\n", delta_time);
        k_msleep(20);
    }
}

K_THREAD_DEFINE(thread0_id, THREAD0_STACKSIZE, thread0, NULL, NULL, NULL,
                THREAD0_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread1_id, THREAD1_STACKSIZE, thread1, NULL, NULL, NULL,
                THREAD1_PRIORITY, 0, 0);
