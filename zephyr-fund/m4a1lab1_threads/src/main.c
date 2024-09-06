/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACKSIZE 1024
#define THREAD0_PRIORITY 5
#define THREAD1_PRIORITY 5

void thread_0(void) {
    int counter = 3;

    while (counter) {
        printk("Hello, I am thread_0 - %d\n", counter--);
        k_msleep(500);
    }
}

void thread_1(void) {
    int counter = 3;

    while (counter) {
        printk("Hello, I am thread_1 - %d\n", counter--);
        k_msleep(500);
    }
}

K_THREAD_DEFINE(thread0_id, STACKSIZE, thread_0, NULL, NULL, NULL,
                THREAD0_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread1_id, STACKSIZE, thread_1, NULL, NULL, NULL,
                THREAD1_PRIORITY, 0, 0);