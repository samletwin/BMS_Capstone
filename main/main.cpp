#include "main_cfg.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "esp_task_wdt.h"

#include "lvgl.h"

#include "soh.h"
#include "hx8357_cfg.h"
#include "hx8357.h"
#include "FT53xx.h"
#include "spi_init.h"
#include "ui_main.h"
#include "lvgl_example.h"
#include "daly_bms_arduino.h"
#include "sd_card_helper.h"
// #include "test_ui.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "MAIN"

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void daly_bms_task(void *pvParameter);
static void lv_gui_main_task(void *pvParameter);
static void lv_gui_update_variables_task(void *pvParameter);

SemaphoreHandle_t xGuiSemaphore;
static bool lvgl_ui_is_init = false;
static Daly_BMS_UART bms;

/* ---------------------------------------------------------------------------------------
    APPLICATION MAIN
 --------------------------------------------------------------------------------------- */
extern "C" void app_main() {
    /* INIT */
#if defined(LOG_TO_SD_CARD) && defined(DISPLAY_CONNECTED)
    ESP_LOGI(TAG, "Free heap before LVGL init: %ld", esp_get_free_heap_size());
    lv_init();
    // Wait a bit for LVGL to initialize
    vTaskDelay(pdMS_TO_TICKS(100));
    spi_full_init(SPI_INIT_BOTH);
#elif defined(LOG_TO_SD_CARD)
    spi_full_init(SPI_INIT_SD_LOG_ONLY);
#elif defined(DISPLAY_CONNECTED)
    ESP_LOGI(TAG, "Free heap before LVGL init: %ld", esp_get_free_heap_size());
    lv_init();
    // Wait a bit for LVGL to initialize
    vTaskDelay(pdMS_TO_TICKS(100));
    spi_full_init(SPI_INIT_DISPLAY_ONLY);
#endif 

    #ifdef CONNECT_TO_DALY
    bms = Daly_BMS_UART();

    if (true != bms.Init())
        ESP_LOGE(TAG, "Error initializing daly BMS");
    xTaskCreate(daly_bms_task, "Daly BMS Task", 4096, NULL, 1, NULL);
    #endif
    

    #ifdef DISPLAY_CONNECTED
    xTaskCreatePinnedToCore(lv_gui_main_task, "gui", 16384, NULL, 5, NULL, 0);
    // xTaskCreatePinnedToCore(lv_gui_update_variables_task, "gui var update task", 2048, NULL, 0, NULL, 1);
    #endif
    
}

static void lv_gui_update_variables_task(void *pvParameter) {
    while (1) {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            xSemaphoreGive(xGuiSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(UPDATE_GUI_VAR_TASK_DELAY_MS));
    }
}


// daly bms update task
static void daly_bms_task(void *pvParameter) {
    while (1) {
        bool retVal = bms.update();
        if (true == retVal) {
            // bms.printBmsStats();
            // bms.printBmsAlarms();
            ESP_LOGI(TAG, "BMS Stats: %.2fV, %.2fA, %.2f perc", bms.get.packVoltage, bms.get.packCurrent, bms.get.packSOC);
        }
        else {
            ESP_LOGE(TAG, "Failed to communicate with DALY BMS");
        }
        vTaskDelay(pdMS_TO_TICKS(DALY_BMS_UPDATE_TASK_DELAY_MS));
    }
}

static void lv_gui_main_task(void *pvParameter) {
    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_color_t* buf1 = (lv_color_t*)heap_caps_malloc((HX8357_BUF_SIZE) * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    lv_color_t* buf2 = (lv_color_t*)heap_caps_malloc((HX8357_BUF_SIZE) * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);

    static lv_display_t* display = lv_display_create(HX8357_TFTHEIGHT, HX8357_TFTWIDTH);
    lv_display_set_flush_cb(display, hx8357_flush);

    lv_display_set_buffers(display, buf1, buf2, HX8357_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* init touch */
    ft53xx_init(FT53XX_DEFAULT_ADDR);
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);   
    lv_indev_set_read_cb(indev, ft53xx_read);  

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Free heap after LVGL init: %ld", esp_get_free_heap_size());

    ui_main_init();
    // ui_main_show();
    lvgl_ui_is_init = true;

    
    while (1) {

        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }

        // Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(5)); // Yield every 5ms
    }

    /* A task should NEVER return */
    free(buf1);
    free(buf2);
    vTaskDelete(NULL);
}

static void lv_tick_task(void *arg) {
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}