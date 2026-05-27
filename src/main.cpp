#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

constexpr gpio_num_t LED_PIN = GPIO_NUM_16;
constexpr gpio_num_t MOTOR_PIN = GPIO_NUM_17;

// ESP32-S3: GPIO1 = ADC1_CH0
constexpr adc_channel_t POT_CHANNEL = ADC_CHANNEL_0;

constexpr adc_unit_t ADC_UNIT = ADC_UNIT_1;
constexpr adc_bitwidth_t ADC_BITWIDTH = ADC_BITWIDTH_12;
constexpr adc_atten_t ADC_ATTEN = ADC_ATTEN_DB_12;

constexpr ledc_mode_t LEDC_MODE = LEDC_LOW_SPEED_MODE;
constexpr ledc_timer_t LEDC_TIMER = LEDC_TIMER_0;
constexpr ledc_timer_bit_t LEDC_RESOLUTION = LEDC_TIMER_8_BIT;
constexpr uint32_t LEDC_FREQUENCY_HZ = 1000;

constexpr ledc_channel_t LED_CHANNEL = LEDC_CHANNEL_0;
constexpr ledc_channel_t MOTOR_CHANNEL = LEDC_CHANNEL_1;

constexpr uint32_t ADC_MAX_VALUE = 4095;
constexpr uint32_t PWM_MAX_DUTY = 255;

constexpr uint32_t READ_DELAY_MS = 50;

static const char* TAG = "PWM_CONTROLLER";

adc_oneshot_unit_handle_t adcHandle;

uint32_t mapAdcToPwm(int adcValue)
{
    if (adcValue < 0) {
        return 0;
    }

    if (adcValue > static_cast<int>(ADC_MAX_VALUE)) {
        return PWM_MAX_DUTY;
    }

    return static_cast<uint32_t>(adcValue) * PWM_MAX_DUTY / ADC_MAX_VALUE;
}

void initAdc()
{
    adc_oneshot_unit_init_cfg_t unitConfig = {};
    unitConfig.unit_id = ADC_UNIT;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unitConfig, &adcHandle));

    adc_oneshot_chan_cfg_t channelConfig = {};
    channelConfig.bitwidth = ADC_BITWIDTH;
    channelConfig.atten = ADC_ATTEN;

    ESP_ERROR_CHECK(adc_oneshot_config_channel(
        adcHandle,
        POT_CHANNEL,
        &channelConfig
    ));
}

void initPwm()
{
    ledc_timer_config_t timerConfig = {};
    timerConfig.speed_mode = LEDC_MODE;
    timerConfig.timer_num = LEDC_TIMER;
    timerConfig.duty_resolution = LEDC_RESOLUTION;
    timerConfig.freq_hz = LEDC_FREQUENCY_HZ;
    timerConfig.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&timerConfig));

    ledc_channel_config_t ledConfig = {};
    ledConfig.gpio_num = LED_PIN;
    ledConfig.speed_mode = LEDC_MODE;
    ledConfig.channel = LED_CHANNEL;
    ledConfig.intr_type = LEDC_INTR_DISABLE;
    ledConfig.timer_sel = LEDC_TIMER;
    ledConfig.duty = 0;
    ledConfig.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&ledConfig));

    ledc_channel_config_t motorConfig = {};
    motorConfig.gpio_num = MOTOR_PIN;
    motorConfig.speed_mode = LEDC_MODE;
    motorConfig.channel = MOTOR_CHANNEL;
    motorConfig.intr_type = LEDC_INTR_DISABLE;
    motorConfig.timer_sel = LEDC_TIMER;
    motorConfig.duty = 0;
    motorConfig.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&motorConfig));
}

void setPwmDuty(ledc_channel_t channel, uint32_t duty)
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, channel));
}

extern "C" void app_main()
{
    initAdc();
    initPwm();

    ESP_LOGI(TAG, "PWM Dual Controller started");

    while (true) {
        int adcValue = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(
            adcHandle,
            POT_CHANNEL,
            &adcValue
        ));

        const uint32_t pwmDuty = mapAdcToPwm(adcValue);

        setPwmDuty(LED_CHANNEL, pwmDuty);
        setPwmDuty(MOTOR_CHANNEL, pwmDuty);

        ESP_LOGI(
            TAG,
            "ADC: %d | PWM Duty: %" PRIu32,
            adcValue,
            pwmDuty
        );

        vTaskDelay(pdMS_TO_TICKS(READ_DELAY_MS));
    }
}