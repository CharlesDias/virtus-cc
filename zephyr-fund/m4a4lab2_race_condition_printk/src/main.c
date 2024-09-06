/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define THREAD_PRIORITY 3

void thread_1_func(void *arg1, void *arg2, void *arg3)
{
    for (volatile int i = 0; i < 5; i++)
    {
        printk("----------------------------------------\n");
    }
}

void thread_2_func(void *arg1, void *arg2, void *arg3)
{
    for (volatile int i = 0; i < 5; i++)
    {
        printk("++++++++++++++++++++++++++++++++++++++++\n");
    }
}

K_THREAD_DEFINE(thread0_id, STACK_SIZE, thread_1_func, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread1_id, STACK_SIZE, thread_2_func, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);