/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>

int main(void)
{
	int int_var = 42;
	long long_var = 1234567890L;
	long long long_long_var = 1234567890123456789LL;
	unsigned long ulong_var = 987654321UL;
	unsigned long long ulong_long_var = 9876543210123456789ULL;
	const char *string_var = "Zephyr RTOS";

	printk("Hello, Zephyr World!\n");
	printk("String: %s (size: %zu bytes + null character)\n", string_var, strlen(string_var));
	printk("Integer: %d (size: %zu bytes)\n", int_var, sizeof(int_var));
	printk("Long: %ld (size: %zu bytes)\n", long_var, sizeof(long_var));
	printk("Long Long: %lld (size: %zu bytes)\n", long_long_var, sizeof(long_long_var));
	printk("Unsigned Long: %lu (size: %zu bytes)\n", ulong_var, sizeof(ulong_var));
	printk("Unsigned Long Long: %llu (size: %zu bytes)\n", ulong_long_var, sizeof(ulong_long_var));
	printk("ERROR Integer: %lld\n", int_var);
	printk("ERROR Unsigned: %llu\n", ulong_var);
	
	return 0;
}
