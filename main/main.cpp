#include "main_cfg.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_freertos_hooks.h"
// #include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

#include "lvgl.h"

#include "soh.h"
#include "daly_bms_serial.h"
#include "hx8357_cfg.h"
#include "hx8357.h"
#include "FT53xx.h"
#include "spi_init.h"
#include "ui_main.h"

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

static SemaphoreHandle_t xGuiSemaphore;
static bool lvgl_ui_is_init = false;
static Daly_BMS_UART bms;
static UIMain* lvgl_ui_main_ptr = nullptr;

/* ---------------------------------------------------------------------------------------
    APPLICATION MAIN
 --------------------------------------------------------------------------------------- */
extern "C" void app_main() {
    /* INIT */
    // bms = Daly_BMS_UART();

    // if (true != bms.Init())
        // ESP_LOGE(TAG, "Error initializing daly BMS");
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the lv_gui_main_task to core 0 */
    xTaskCreatePinnedToCore(lv_gui_main_task, "gui", 4096*2, NULL, 1, NULL, 0);
    // xTaskCreate(daly_bms_task, "Daly BMS Task", 4096, NULL, 1, NULL);
    // xTaskCreatePinnedToCore(lv_gui_update_variables_task, "gui var update task", 2048, NULL, 0, NULL, 1);
}

static void lv_gui_update_variables_task(void *pvParameter) {
    while (1) {
        if (true == lvgl_ui_is_init) {
            if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
                // tick_screen(SCREEN_ID_MAIN_SINGLE_CELL); // TODO: update screens.c to tick active screen - for now this is sufficient
                xSemaphoreGive(xGuiSemaphore);
            }
        }
        else
            ESP_LOGD(TAG, "Not updating GUI vars because LVGL UI has not been init.");
        vTaskDelay(pdMS_TO_TICKS(UPDATE_GUI_VAR_TASK_DELAY_MS));
    }
}


// daly bms update task
static void daly_bms_task(void *pvParameter) {
    while (1) {
        bool retVal = bms.updateSpecific();
        if (true == retVal) {
            // bms.printBmsStats();
            // bms.printBmsAlarms();
        }
        else {
            ESP_LOGE(TAG, "Failed to communicate with DALY BMS");
        }
        vTaskDelay(pdMS_TO_TICKS(DALY_BMS_UPDATE_TASK_DELAY_MS));
    }
}


static void lv_gui_main_task(void *pvParameter) {
    ESP_LOGD(TAG, "entering lv gui main task");
    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();
    ESP_LOGD(TAG, "about to start lv_init");
    lv_init();
    // Wait a bit for LVGL to initialize
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGD(TAG, "about to init  spi");
    /* Initialize SPI or I2C bus used by the drivers */
    spi_full_init(SPI_INIT_DISPLAY_ONLY);

    lv_color_t* buf1 = (lv_color_t*)heap_caps_malloc(HX8357_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    lv_color_t* buf2 = (lv_color_t*)heap_caps_malloc(HX8357_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
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

    // Create UIMain instance
    lvgl_ui_main_ptr = new UIMain();
    if (lvgl_ui_main_ptr == nullptr) {
        ESP_LOGE(TAG, "Failed to create UIMain instance");
        return;
    }

    // Initialize UIMain
    if (!lvgl_ui_main_ptr->init()) {
        ESP_LOGE(TAG, "Failed to initialize UIMain");
        delete lvgl_ui_main_ptr;
        return;
    }

    // Show the UI
    lvgl_ui_main_ptr->show();

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
    free(buf2);
    vTaskDelete(NULL);
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
