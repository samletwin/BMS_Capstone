/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_vars.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

#include "ui/ui.h"
#include "adc.h"
#include "gpio.h"
#include "soh.h"
#include "timing.h"

#include "main_cfg.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "MAIN"


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void adc_read_task(void *pvParameter);
static void gui_to_controller_task(void *pvParameter);
static void lv_gui_main_task(void *pvParameter);
static void lv_gui_update_variables_task(void *pvParameter);

static void start_soh_measurement();

static SemaphoreHandle_t xGuiSemaphore;
static uint16_t * adc_buffer_bat_volt_mV_aui16;
static uint16_t * adc_buffer_bat_cur_mA_aui16;
static uint16_t * adc_buffer_timestamp_ms_aui16;
static bool lvgl_ui_is_init = false;
static soh_result res = (soh_result){0};

static bool adc_log_readings_flag_b = false;

/* ---------------------------------------------------------------------------------------
    APPLICATION MAIN
 --------------------------------------------------------------------------------------- */
void app_main() {
    /* INIT */
    adc_init();
    gpio_init();
    timer_initialize();
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the lv_gui_main_task to core 0 */
    xTaskCreatePinnedToCore(lv_gui_main_task, "gui", 4096*2, NULL, 1, NULL, 1);
    
    xTaskCreate(adc_read_task, "ADC Read Task", 4096, NULL, 1, NULL);
    xTaskCreatePinnedToCore(lv_gui_update_variables_task, "gui var update task", 2048, NULL, 0, NULL, 1);
    xTaskCreate(gui_to_controller_task, "gui to controller task", 2048, NULL, 1, NULL);
}

static void lv_gui_update_variables_task(void *pvParameter) {
    while (1) {
        if (true == lvgl_ui_is_init) {
            if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
                tick_screen(SCREEN_ID_MAIN_SINGLE_CELL); // TODO: update screens.c to tick active screen - for now this is sufficient
                xSemaphoreGive(xGuiSemaphore);
            }
        }
        else
            ESP_LOGD(TAG, "Not updating GUI vars because LVGL UI has not been init.");
        vTaskDelay(pdMS_TO_TICKS(UPDATE_GUI_VAR_TASK_DELAY_MS));
    }
}


//TODO: general adc read - clean up to only use timer read
static void adc_read_task(void *pvParameter) {
    while (1) {
        if (false==adc_log_readings_flag_b) {
        // Read ADC values
            uint16_t adc_batteryVoltage_mV_ui16 = adc_readBattVoltage_mV(false);
            uint16_t adc_batteryCurrent_mA_ui16 = adc_readBattCurrent_mA(false);
            set_display_adcBatCurrent_mA_ui16(adc_batteryCurrent_mA_ui16);
            set_display_adcBatVolt_mV_ui16(adc_batteryVoltage_mV_ui16);
            set_display_batRes_f32(res.internalResistance_f32);
            set_display_batOcv_f32(res.OCV_f32);
        }
        // }
        
        // Check and print the stack high water mark
        // UBaseType_t stack_high_water_mark = uxTaskGetStackHighWaterMark(NULL);
        // ESP_LOGD(TAG, "ADC Read Task - Stack high water mark: %d\n", stack_high_water_mark);

        // Delay for the specified interval
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void gpio_toggle_soh_timer_callback(void *arg) {
    periodicTimerType *periodic_timer = (periodicTimerType *)arg;
    if (NULL == periodic_timer) {
        ESP_LOGE("PERIODIC_TIMER", "Args for SOH timer callback are NULL");
        return;
    }

    // Toggle GPIO pin
    int pin_state = gpio_get_level(GPIO_SWITCH_BATTERY_DISCHARGE_OUT);
    gpio_set_battery_discharge_switch(!pin_state, false);

    // Increment the toggle count
    (periodic_timer->toggle_count_ui16)++;

    ESP_LOGD("PERIODIC_TIMER", "GPIO toggled. Count: %d", periodic_timer->toggle_count_ui16);

    // Check if we reached the maximum number of toggles
    if (periodic_timer->toggle_count_ui16 >= periodic_timer->max_toggles_ui16) {
        ESP_LOGI("PERIODIC_TIMER", "Maximum toggles reached. Deleting timer.");

        ESP_ERROR_CHECK(delete_periodic_timer(periodic_timer));
    }
}

static void adc_periodic_timer_callback(void *arg) {
    periodicTimerType *periodic_timer = (periodicTimerType *)arg;
    if (NULL == periodic_timer) {
        ESP_LOGE("PERIODIC_TIMER", "Args for SOH timer callback are NULL");
        return;
    }
    uint16_t adc_batteryVoltage_mV_ui16 = adc_readBattVoltage_mV(false);
    uint16_t adc_batteryCurrent_mA_ui16 = adc_readBattCurrent_mA(false);
    adc_buffer_bat_volt_mV_aui16[periodic_timer->toggle_count_ui16] = adc_batteryVoltage_mV_ui16;
    adc_buffer_bat_cur_mA_aui16[periodic_timer->toggle_count_ui16] = adc_batteryCurrent_mA_ui16;
    adc_buffer_timestamp_ms_aui16[periodic_timer->toggle_count_ui16] = get_timer_value_ms();
    periodic_timer->toggle_count_ui16 += 1;
    if (periodic_timer->toggle_count_ui16 >= periodic_timer->max_toggles_ui16) {
        adc_log_readings_flag_b = false;
        ESP_LOGD(TAG, "ADC Buffer:");
        for (uint16_t i = 0; i < periodic_timer->max_toggles_ui16; i++) {
            ESP_LOGD(TAG, "Volt = %u mV, Current = %u mA, Time = %u ms", adc_buffer_bat_volt_mV_aui16[i],
                adc_buffer_bat_cur_mA_aui16[i], adc_buffer_timestamp_ms_aui16[i]);
        }
        stop_timer();
        res = soh_LeastSquares(adc_buffer_bat_volt_mV_aui16, adc_buffer_bat_cur_mA_aui16, periodic_timer->max_toggles_ui16, false);
        set_display_batRes_f32(res.internalResistance_f32);
        set_display_batOcv_f32(res.OCV_f32);
        heap_caps_free(adc_buffer_bat_volt_mV_aui16);
        heap_caps_free(adc_buffer_bat_cur_mA_aui16);
        heap_caps_free(adc_buffer_timestamp_ms_aui16);
        ESP_LOGI(TAG, "Freed allocated memory for SOH Measurement");
        ESP_ERROR_CHECK(delete_periodic_timer(periodic_timer));
        adc_log_readings_flag_b = false;
        set_UI_sohMeasurementStatus_e(UI_NO_MEASUREMENT);
    }
}

static void start_soh_measurement() {
    const soh_configDataType * sohConfigData_s = get_all_sohConfigData_ps();

    /* Allocate memory for logging specified number of samples */
    uint16_t numSamples_ui16 = (sohConfigData_s->numDischarges_ui8 * sohConfigData_s->dischargePeriod_ms_ui16 * sohConfigData_s->sampleRate_hz_ui16) / 1000;
    uint16_t size_ui16 = numSamples_ui16 * sizeof(uint16_t);
    adc_buffer_bat_volt_mV_aui16 = (uint16_t*)heap_caps_malloc(size_ui16, MALLOC_CAP_DMA);
    adc_buffer_bat_cur_mA_aui16 = (uint16_t*)heap_caps_malloc(size_ui16, MALLOC_CAP_DMA);
    adc_buffer_timestamp_ms_aui16 = (uint16_t*)heap_caps_malloc(size_ui16, MALLOC_CAP_DMA);
    ESP_LOGI(TAG, "Allocated %u Bytes of memory for SOH Measurment", size_ui16*3);

    /* Create timer for toggling battery discharge switch - timer is deleted in callback function once finished */
    periodicTimerType soh_periodic_timer = {
        .timer_handle = NULL,
        .timer_interval_us_ui64 = (sohConfigData_s->dischargePeriod_ms_ui16 * 1000)/2,
        .timer_name = "soh_periodic_timer",
        .timer_type = TYPE_TOGGLE,
        .user_callback = &gpio_toggle_soh_timer_callback,
        .max_toggles_ui16 = (sohConfigData_s->numDischarges_ui8)*2, 
        .toggle_count_ui16 = 0
    };
    ESP_ERROR_CHECK(create_periodic_timer(&soh_periodic_timer));

    /* Create timer for logging adc values at specific interval */
    periodicTimerType adc_logging_timer = {
        .timer_handle = NULL,
        .timer_interval_us_ui64 = (1000000/sohConfigData_s->sampleRate_hz_ui16),
        .timer_name = "adc_logging_timer",
        .timer_type = TYPE_NORMAL,
        .user_callback = &adc_periodic_timer_callback,
        .max_toggles_ui16 = numSamples_ui16, 
        .toggle_count_ui16 = 0
    };
    ESP_ERROR_CHECK(create_periodic_timer(&adc_logging_timer));

    /* start timer for timestamps for adc logging*/
    start_timer(); /*TODO: remove this - isnt needed */
    adc_log_readings_flag_b = true;
}


/* Task to handle controller functionality based on user input form GUI */
static void gui_to_controller_task(void *pvParameter) {
    while (1) {
        const ui_eventDataType* gui_eventData_ps = get_all_UI_eventData_ps();
        
        if (gui_eventData_ps->sohMeasurementStatus_e == UI_START_MEASUREMENT) {
            set_UI_sohMeasurementStatus_e(UI_MEASUREMENT_IN_PROGRESS);
            start_soh_measurement();
        }
        /* Just start discharging ONLY if we are not starting SOH Measurement */
        else if (gui_eventData_ps->dischargeBattSwitch_e == UI_SWITCH_ON) {
            gpio_set_battery_discharge_switch(GPIO_HIGH, false);
        }
        else if (gui_eventData_ps->dischargeBattSwitch_e == UI_SWITCH_OFF) {
            gpio_set_battery_discharge_switch(GPIO_LOW, false);
        }

        vTaskDelay(pdMS_TO_TICKS(GUI_TO_CONTROLLER_TASK_DELAY_MS));
    }
}

static void lv_gui_main_task(void *pvParameter) {

    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t* buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820         \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A    \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D     \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

#if defined CONFIG_DISPLAY_ORIENTATION_PORTRAIT || defined CONFIG_DISPLAY_ORIENTATION_PORTRAIT_INVERTED
    disp_drv.rotated = 1;
#endif

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* init the ui */
    ui_init();
    lvgl_ui_is_init = true;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(GUI_TASK_DELAY_MS));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
    vTaskDelete(NULL);
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}