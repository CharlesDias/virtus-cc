/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define THREAD_PRIORITY 3
#define SLEEPTIME 1000

K_SEM_DEFINE(thread1_sem, 1, 1);  // Inicializado em 1, para que a thread 1 inicie
K_SEM_DEFINE(thread2_sem, 0, 1);  // Inicializado em 0, para que a thread 2 espere

void thread_1_func(void *arg1, void *arg2, void *arg3)
{
    for (volatile int i = 0; i < 5; i++)
    {
        k_sem_take(&thread1_sem, K_FOREVER);
        printk("Thread 1: Executando\n");      
        k_sem_give(&thread2_sem);
        k_msleep(SLEEPTIME);
    }
}

void thread_2_func(void *arg1, void *arg2, void *arg3)
{
    for (volatile int i = 0; i < 5; i++)
    {
        k_sem_take(&thread2_sem, K_FOREVER);
         printk("Thread 2: Executando\n");
        k_sem_give(&thread1_sem);
        k_msleep(SLEEPTIME);
    }
}

K_THREAD_DEFINE(thread0_id, STACK_SIZE, thread_1_func, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread1_id, STACK_SIZE, thread_2_func, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);



