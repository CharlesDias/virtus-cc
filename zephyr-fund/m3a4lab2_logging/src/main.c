/*
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

// Define um módulo de logging. O nome do módulo deve ser único.
LOG_MODULE_REGISTER(my_module, LOG_LEVEL_DBG);

void some_function(void) {
    int result = 10;
	int arr[] = {'a', 'b', 'c', 'd', 'e'};

    LOG_DBG("Resultado: %d", result);
	LOG_HEXDUMP_DBG(arr, sizeof(arr), "Array de inteiros");
}

int main(void) {
	int x = 42;

    LOG_ERR("Erro: algo deu errado!");
    LOG_WRN("Aviso: algo pode estar errado!");
    LOG_INF("Info: sistema iniciado com sucesso.");
    LOG_DBG("Debug: valor de x = %d", x);

    some_function();

	return 0;
}


