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
	float float_var = 123.456f;
	double double_var = 123456.7890123;

	// Imprimindo um valor float
	printk("Float: %.2f\n", float_var); // Impressão com 2 casas decimais

	// Imprimindo um valor double
	printk("Double: %.5lf\n", double_var); // Impressão com 5 casas decimais

	return 0;
}