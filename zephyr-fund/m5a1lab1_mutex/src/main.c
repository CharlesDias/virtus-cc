/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define THREAD_PRIORITY 3

K_THREAD_STACK_DEFINE(thread_1_stack_area, STACK_SIZE);
static struct k_thread thread_1;

K_THREAD_STACK_DEFINE(thread_2_stack_area, STACK_SIZE);
static struct k_thread thread_2;

volatile int counter = 0;

K_MUTEX_DEFINE(mutex_counter);

void increment_counter(void *arg1, void *arg2, void *arg3)
{
    k_mutex_lock(&mutex_counter, K_FOREVER);

    for (volatile int i = 0; i < 1000000; i++)
    {
        counter++;
    }

    k_mutex_unlock(&mutex_counter);
}

int main(void)
{
    k_thread_create(&thread_1, thread_1_stack_area,
                    K_THREAD_STACK_SIZEOF(thread_1_stack_area),
                    increment_counter, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_FOREVER);
    

    k_thread_create(&thread_2, thread_2_stack_area,
                    K_THREAD_STACK_SIZEOF(thread_2_stack_area),
                    increment_counter, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_FOREVER);

    k_thread_start(&thread_1);
    k_thread_start(&thread_2);


    k_thread_join(&thread_1, K_FOREVER);
    k_thread_join(&thread_2, K_FOREVER);

    printk("Counter: %d\n", counter);

    return 0;
}