/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

#define THREAD_PRIORITY_1 3
#define THREAD_PRIORITY_2 2
#define THREAD_PRIORITY_3 1

void thread_entry(void *arg1, void *arg2, void *arg3)
{
    char * thread_name = (char *)arg1;
    uint64_t time_stamp;
    int64_t delta_time;

    time_stamp = k_uptime_get();
    printk("%4lld - %s starting\n", time_stamp, thread_name);
    for(volatile int count_out = 0; count_out < 1000000; count_out ++); // Simulando trabalho
    delta_time = k_uptime_delta(&time_stamp);

    printk("%4lld - %s takes around %lld ms\n", k_uptime_get(), thread_name, delta_time);
}

K_THREAD_DEFINE(thread_1_id, STACK_SIZE, thread_entry, "Thread 1", NULL, NULL,
                THREAD_PRIORITY_1, 0, 0);

K_THREAD_DEFINE(thread_2_id, STACK_SIZE, thread_entry, "Thread 2", NULL, NULL,
                THREAD_PRIORITY_2, 0, 50);

K_THREAD_DEFINE(thread_3_id, STACK_SIZE, thread_entry, "Thread 3", NULL, NULL,
                THREAD_PRIORITY_3, 0, 100);