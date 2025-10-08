#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

// PINOS ATUALIZADOS CONFORME SEU DIAGRAMA
#define LED_PIN_0   (gpio_num_t)20
#define LED_PIN_1   (gpio_num_t)19
#define LED_PIN_2   (gpio_num_t)48
#define LED_PIN_3   (gpio_num_t)47
#define BTN_A_PIN   (gpio_num_t)14
#define BTN_B_PIN   (gpio_num_t)13

static const char *TAG = "LED_COUNTER";
#define DEBOUNCE_TIME_US 50000

volatile uint8_t counter = 0;
volatile uint8_t increment_step = 1;

const gpio_num_t led_pins[4] = {LED_PIN_0, LED_PIN_1, LED_PIN_2, LED_PIN_3};

typedef struct {
    gpio_num_t pin;
    bool last_stable_state;
    int64_t last_debounce_time;
} Button;

Button button_a = { .pin = BTN_A_PIN, .last_stable_state = true, .last_debounce_time = 0 };
Button button_b = { .pin = BTN_B_PIN, .last_stable_state = true, .last_debounce_time = 0 };

void update_leds() {
    for (int i = 0; i < 4; i++) {
        bool level = (counter >> i) & 1;
        gpio_set_level(led_pins[i], level);
    }
}

bool handle_button_press(Button *btn) {
    bool press_detected = false;
    bool current_state = gpio_get_level(btn->pin);

    if (current_state != btn->last_stable_state) {
        if ((esp_timer_get_time() - btn->last_debounce_time) > DEBOUNCE_TIME_US) {
            if (current_state == 0) {
                press_detected = true;
            }
            btn->last_stable_state = current_state;
            btn->last_debounce_time = esp_timer_get_time();
        }
    }
    return press_detected;
}

void counter_task(void *pvParameters) {
    ESP_LOGI(TAG, "Tarefa do contador iniciada. Incremento inicial: %d", increment_step);
    update_leds(); 

    while (1) {
        if (handle_button_press(&button_a)) {
            counter = (counter + increment_step) % 16;
            ESP_LOGI(TAG, "Botao A: Contador incrementado para -> %d (0x%X)", counter, counter);
            update_leds();
        }

        if (handle_button_press(&button_b)) {
            increment_step = (increment_step == 1) ? 2 : 1;
            ESP_LOGI(TAG, "Botao B: Passo de incremento alterado para -> %d", increment_step);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

#ifdef ARDUINO_ARCH_ESP32
void setup() {
  Serial.begin(115200);
  
  // --- CONFIGURAÇÃO CORRIGIDA ---

  // 1. Configuração exclusiva para os LEDs (Saídas)
  gpio_config_t led_conf = {}; // Cria uma struct limpa para os LEDs
  led_conf.pin_bit_mask = ((1ULL << LED_PIN_0) | (1ULL << LED_PIN_1) | (1ULL << LED_PIN_2) | (1ULL << LED_PIN_3));
  led_conf.mode = GPIO_MODE_OUTPUT;
  led_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  led_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  led_conf.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&led_conf);
    
  // 2. Configuração exclusiva para os Botões (Entradas)
  gpio_config_t btn_conf = {}; // Cria uma struct limpa para os botões
  btn_conf.pin_bit_mask = ((1ULL << BTN_A_PIN) | (1ULL << BTN_B_PIN));
  btn_conf.mode = GPIO_MODE_INPUT;
  btn_conf.pull_up_en = GPIO_PULLUP_ENABLE; // Habilita resistor de pull-up
  btn_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  btn_conf.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&btn_conf);

  ESP_LOGI(TAG, "Configuracao de GPIOs concluida.");

  xTaskCreate(counter_task, "counter_task", 4096, NULL, 5, NULL);
}

void loop() {
  vTaskDelay(1000); 
}

#else
void app_main(void) {
    // 1. Configuração exclusiva para os LEDs (Saídas)
    gpio_config_t led_conf = {}; // Cria uma struct limpa para os LEDs
    led_conf.pin_bit_mask = ((1ULL << LED_PIN_0) | (1ULL << LED_PIN_1) | (1ULL << LED_PIN_2) | (1ULL << LED_PIN_3));
    led_conf.mode = GPIO_MODE_OUTPUT;
    led_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    led_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    led_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&led_conf);
    
    // 2. Configuração exclusiva para os Botões (Entradas)
    gpio_config_t btn_conf = {}; // Cria uma struct limpa para os botões
    btn_conf.pin_bit_mask = ((1ULL << BTN_A_PIN) | (1ULL << BTN_B_PIN));
    btn_conf.mode = GPIO_MODE_INPUT;
    btn_conf.pull_up_en = GPIO_PULLUP_ENABLE; // Habilita resistor de pull-up
    btn_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    btn_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&btn_conf);

    ESP_LOGI(TAG, "Configuracao de GPIOs concluida.");

    xTaskCreate(counter_task, "counter_task", 4096, NULL, 5, NULL);
}
#endif
