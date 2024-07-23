#include "main_cfg.h"

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

#include "lvgl.h"


#include "ui/ui.h"
#include "soh.h"
#include "daly_bms_serial.h"
#include "hx8357_cfg.h"
#include "hx8357.h"
#include "spi_init.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "MAIN"


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void daly_bms_task(void *pvParameter);
static void gui_to_controller_task(void *pvParameter);
static void lv_gui_main_task(void *pvParameter);
static void lv_gui_update_variables_task(void *pvParameter);

static SemaphoreHandle_t xGuiSemaphore;
static bool lvgl_ui_is_init = false;
static Daly_BMS_UART bms;
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
    xTaskCreatePinnedToCore(lv_gui_main_task, "gui", 4096*2, NULL, 1, NULL, 1);
    // xTaskCreate(daly_bms_task, "Daly BMS Task", 4096, NULL, 1, NULL);
    xTaskCreatePinnedToCore(lv_gui_update_variables_task, "gui var update task", 2048, NULL, 0, NULL, 1);
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

    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    spi_full_init(0);

    lv_color_t* buf1 = (lv_color_t*)heap_caps_malloc(HX8357_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    lv_color_t* buf2 = (lv_color_t*)heap_caps_malloc(HX8357_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = HX8357_BUF_SIZE;


    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = hx8357_flush;


    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* init touch */
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

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
