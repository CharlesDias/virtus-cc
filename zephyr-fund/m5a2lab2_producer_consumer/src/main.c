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

int buffer[BUFFER_SIZE];
int count = 0;  // Contador de itens no buffer

K_SEM_DEFINE(producer_sem, 1, 1);
K_SEM_DEFINE(consumer_sem, 0, 1);

void producer(void *arg1, void *arg2, void *arg3)
{
    int i = 0;

    while (1)
    {
        k_sem_take(&producer_sem, K_FOREVER);
        
        // Adiciona dados ao buffer
        buffer[count] = i;
        printk("Produtor: adicionou %d no buffer\n", buffer[count]);
        count++;
        i++;

        k_sem_give(&consumer_sem);
        k_msleep(500);
    }
}

void consumer(void *arg1, void *arg2, void *arg3)
{
    while (1)
    {
        k_sem_take(&consumer_sem, K_FOREVER);
        
        // Remove dados ao buffer
        count--;
        int item = buffer[count];
        printk("Consumidor: removeu %d do buffer\n", item);

        k_sem_give(&producer_sem);
        k_msleep(500);
    }
}

K_THREAD_DEFINE(producer_id, STACK_SIZE, producer, NULL, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(consumer_id, STACK_SIZE, consumer, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);



