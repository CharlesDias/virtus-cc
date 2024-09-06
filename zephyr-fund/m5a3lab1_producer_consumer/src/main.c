/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRODUCER_PRIORITY 4
#define CONSUMER_PRIORITY 3
#define BUFFER_SIZE 5

K_MSGQ_DEFINE(msgq, sizeof(int), BUFFER_SIZE, 1);

void producer(void *arg1, void *arg2, void *arg3)
{
    int item = 0;

    while (1)
    {
        if (k_msgq_put(&msgq, &item, K_FOREVER) == 0)
        {
            printk("Produtor adicionou: %d\n", item);
            item++;
        }
        k_msleep(500);
    }
}

void consumer(void *arg1, void *arg2, void *arg3)
{
    int item;

    while (1)
    {
        if (k_msgq_get(&msgq, &item, K_FOREVER) == 0)
        {
            printk("Consumidor removeu: %d\n", item);
        }
    }
}

K_THREAD_DEFINE(producer_id, STACK_SIZE, producer, NULL, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(consumer_id, STACK_SIZE, consumer, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);
