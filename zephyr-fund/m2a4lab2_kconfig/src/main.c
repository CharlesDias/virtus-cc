/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    #ifdef CONFIG_EXAMPLE_BOOL
        printk("Example boolean variable is enabled.\n");
    #else
        printk("Example boolean variable is disabled.\n");
    #endif

    printk("Example integer variable value: %d\n", CONFIG_EXAMPLE_INT);

	return 0;
}