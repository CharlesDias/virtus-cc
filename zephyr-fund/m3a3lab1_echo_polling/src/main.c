/*
 * Copyright (c) 2024 Charles Dias VIRTUS/UFCG
  *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

int main(void)
{
	uint8_t data;
	int ret;

	if (!device_is_ready(uart_dev))
	{
		printk("Dispositivo UART não encontrado!\n");
		return 0;
	}

	while (1)
	{
		/* Poll para receber dados */
		ret = uart_poll_in(uart_dev, &data);
		if (ret == 0)
		{
			/* Dado recebido com sucesso */
			printk("\nRecebido: %c\n", data);

			/* Poll para enviar dados */
			uart_poll_out(uart_dev, data);
		}
		else
		{
			/* Nenhum dado disponível, aguardar um pouco */
			k_msleep(100);
		}
	}

	return 0;
}
