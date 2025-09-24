#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1 GPIO_NUM_5
#define LED2 GPIO_NUM_18
#define LED3 GPIO_NUM_19
#define LED4 GPIO_NUM_21

#define DELAY_MS 500

void app_main(void) {
    // Configuração das GPIOs como saída
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED4, GPIO_MODE_OUTPUT);

    // Inicia todos apagados
    gpio_set_level(LED1, 0);
    gpio_set_level(LED2, 0);
    gpio_set_level(LED3, 0);
    gpio_set_level(LED4, 0);

    while (1) {
        // --------------------
        // Fase 1: Contador Binário (0 a 15)
        // --------------------
        for (int i = 0; i < 16; i++) {
            if (i & 1) gpio_set_level(LED1, 1); else gpio_set_level(LED1, 0);
            if (i & 2) gpio_set_level(LED2, 1); else gpio_set_level(LED2, 0);
            if (i & 4) gpio_set_level(LED3, 1); else gpio_set_level(LED3, 0);
            if (i & 8) gpio_set_level(LED4, 1); else gpio_set_level(LED4, 0);

            vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
        }

        // --------------------
        // Fase 2: Sequência de Varredura (ida e volta)
        // --------------------
        // LED1 → LED2 → LED3 → LED4
        gpio_set_level(LED1, 1); vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); gpio_set_level(LED1, 0);
        gpio_set_level(LED2, 1); vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); gpio_set_level(LED2, 0);
        gpio_set_level(LED3, 1); vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); gpio_set_level(LED3, 0);
        gpio_set_level(LED4, 1); vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); gpio_set_level(LED4, 0);

        // LED4 → LED3 → LED2 → LED1
        gpio_set_level(LED4, 1); vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); gpio_set_level(LED4, 0);
        gpio_set_level(LED3, 1); vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); gpio_set_level(LED3, 0);
        gpio_set_level(LED2, 1); vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); gpio_set_level(LED2, 0);
        gpio_set_level(LED1, 1); vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); gpio_set_level(LED1, 0);
    }
}
