/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/random/random.h>

#define STACK_SIZE 1024
#define PRODUCER_PRIORITY 4
#define CONSUMER_PRIORITY 3
#define MAX_MESSAGES 10

struct my_data
{
    char ch;
    int value;
    int producer_id;
};

K_MSGQ_DEFINE(msgq, sizeof(struct my_data), MAX_MESSAGES, 1);

void producer(void *arg1, void *arg2, void *arg3)
{
    int producer_id = (int)arg1;
    struct my_data data = {
        .ch = 'A',
        .value = 1,
        .producer_id = producer_id};

    while (1)
    {
        if (k_msgq_put(&msgq, &data, K_FOREVER) == 0)
        {
            printk("Produtor %d enviou:    %c - %2d\n",
                   data.producer_id, data.ch, data.value);
            data.ch = data.value % 26 + 'A';
            data.value++;
        }
        k_msleep(500 + sys_rand32_get() % 1000);
    }
}

void consumer(void *arg1, void *arg2, void *arg3)
{
    while (1)
    {
        struct my_data data;
        if (k_msgq_get(&msgq, &data, K_FOREVER) == 0)
        {
            printk("Consumidor recebeu do Produtor %d: %c - %2d\n",
                   data.producer_id, data.ch, data.value);
        }
    }
}

K_THREAD_DEFINE(producer_id_1, STACK_SIZE, producer, (void *)1, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(producer_id_2, STACK_SIZE, producer, (void *)2, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(producer_id_3, STACK_SIZE, producer, (void *)3, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);

K_THREAD_DEFINE(consumer_id, STACK_SIZE, consumer, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);