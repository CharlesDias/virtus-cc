/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/random/random.h>
#include <stdio.h>

#define STACK_SIZE 1024
#define PRODUCER_PRIORITY 4
#define CONSUMER_PRIORITY 3
#define MAX_DATA_SIZE 32
#define MIN_DATA_ITEMS 2
#define MAX_DATA_ITEMS 5

struct item
{
    void *fifo_reserved; // Reservado pelo Zephyr para uso interno
    uint8_t data[MAX_DATA_SIZE];
    uint16_t len;
};

K_FIFO_DEFINE(my_fifo);

void producer(void *arg1, void *arg2, void *arg3)
{
    int data_count = 0;

    while (1)
    {
        int bytes_written;
        int num_items = MIN_DATA_ITEMS + sys_rand32_get() %
                                             (MAX_DATA_ITEMS - MIN_DATA_ITEMS + 1);
        for (int i = 0; i < num_items; i++)
        {
            struct item *new_item = k_malloc(sizeof(struct item));

            if (new_item != NULL)
            {
                bytes_written = snprintf(new_item->data, MAX_DATA_SIZE, "Data %u:\t%u",
                                         data_count, sys_rand32_get());
                new_item->len = bytes_written;
                data_count++;
                k_fifo_put(&my_fifo, new_item);
            }
        }
        printk("Produtor adicionou %d itens\n", num_items);
        k_msleep(5000);
    }
}

void consumer(void *arg1, void *arg2, void *arg3)
{
    while (1)
    {
        struct item *received_item = k_fifo_get(&my_fifo, K_FOREVER);
        printk("Consumidor item: %s \ttamanho: %u\n", received_item->data,
               received_item->len);
        k_free(received_item);
    }
}

K_THREAD_DEFINE(producer_id, STACK_SIZE, producer, NULL, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(consumer_id, STACK_SIZE, consumer, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);
