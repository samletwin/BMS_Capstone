/**
 * @file FT5336.c
 * @author Sam Letwin (letwins@uwindsor.ca)
 * @brief 
 * @version 0.1
 * @date 2024-06-10
 * 
 * 
 */

#include <esp_log.h>
#include <lvgl.h>
#include "FT53xx.h"
#include "FT53xx_cfg.h"
#include "i2c_helper.h"
#include "i2c_helper_cfg.h"
#include "hx8357.h"

#define TAG "FT5336"


ft53xx_status_t ft53xx_status;
uint8_t current_dev_addr;       // set during init

esp_err_t ft53xx_i2c_read8(uint8_t slave_addr, uint8_t register_addr, uint8_t *data_buf) {
    return i2c_read(I2C_PORT, slave_addr, register_addr, data_buf, 1);
}


/**
  * @brief  Read the FT53xx gesture ID. Initialize first!
  * @param  dev_addr: I2C FT53xx Slave address.
  * @retval The gesture ID or 0x00 in case of failure
  */
uint8_t ft53xx_get_gesture_id() {
    if (!ft53xx_status.inited) {
        ESP_LOGE(TAG, "Init first!");
        return 0x00;
    }
    uint8_t data_buf;
    esp_err_t ret;
    if ((ret = ft53xx_i2c_read8(current_dev_addr, FT5336_GEST_ID, &data_buf) != ESP_OK))
        ESP_LOGE(TAG, "Error reading from device: %s", esp_err_to_name(ret));
    return data_buf;
}

/**
  * @brief  Initialize for FT53xx communication via I2C
  * @param  dev_addr: Device address on communication Bus (I2C slave address of FT53xx).
  * @retval None
  */
void ft53xx_init(uint16_t dev_addr) {
    if (false == i2c_get_init_status())
        i2c_init(I2C_PORT);
    ft53xx_status.inited = true;
    current_dev_addr = dev_addr;
    uint8_t data_buf;
    esp_err_t ret;
    if ((ret = ft53xx_i2c_read8(dev_addr, FT53XX_REG_CHIPID, &data_buf) != ESP_OK))
        ESP_LOGE(TAG, "Error reading from device: %s", esp_err_to_name(ret));    // Only show error the first time
    ESP_LOGI(TAG, "\tDevice ID: 0x%02x", data_buf);

    ft53xx_i2c_read8(dev_addr, FT53XX_REG_VENDID, &data_buf);
    ESP_LOGI(TAG, "\tChip ID: 0x%02x", data_buf);

    ft53xx_i2c_read8(dev_addr, FT53XX_REG_FIRMVERS, &data_buf);
    ESP_LOGI(TAG, "\tFirmware ID: 0x%02x", data_buf);

    ft53xx_i2c_read8(dev_addr, FT53XX_REG_VENDID, &data_buf);
    ESP_LOGI(TAG, "\tPanel ID: 0x%02x", data_buf);

}

/**
  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
  * @param  indev:
  * @param  data: Store data here
  * @retval Always false
  */
void ft53xx_read(lv_indev_t  *indev, lv_indev_data_t *data) {
    if (!ft53xx_status.inited) {
        ESP_LOGE(TAG, "Init first!");
    }
    uint8_t data_buf[32];        // 1 byte status, 2 bytes X, 2 bytes Y
    uint8_t write_buf = 0;
    static int16_t last_x = 0;  // 12bit pixel value
    static int16_t last_y = 0;  // 12bit pixel value

    /* Following how Adafruit_FT5336 library got touch data */
    // Write nothing first
    esp_err_t ret = i2c_write(I2C_PORT, current_dev_addr, I2C_NO_REG, &write_buf, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error writing to touch IC: %s", esp_err_to_name(ret));
    }
    ret = i2c_read(I2C_PORT, current_dev_addr, I2C_NO_REG, &data_buf[0], 32);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error reading from touch IC: %s", esp_err_to_name(ret));
    }
    uint8_t touch_pnt_cnt = data_buf[FT5336_TD_STATUS];  // Number of detected touch points

    if (ret != ESP_OK || touch_pnt_cnt != 1) {    // ignore no touch & multi touch
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;
    }

    last_x = ((data_buf[FT5336_TOUCH1_XH] & 0xF) << 8) | (data_buf[FT5336_TOUCH1_XL]);
    last_y = ((data_buf[FT5336_TOUCH1_YH] & 0xF) << 8) | (data_buf[FT5336_TOUCH1_YL]);

#if (CONFIG_LV_FT53XX_SWAPXY == 1)
    int16_t swap_buf = last_x;
    last_x = last_y;
    last_y = swap_buf;
#endif
#if (CONFIG_LV_FT53XX_INVERT_X == 1)
    last_x =  LV_HOR_RES - last_x;
#endif
#if (CONFIG_LV_FT53XX_INVERT_Y == 1)
    last_y = LV_VER_RES - last_y;
#endif

    if (data->point.x == last_x && data->point.y == last_y) {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    else {
        data->state = LV_INDEV_STATE_PRESSED;
    }
    
    if (last_x <= HX8357_TFTHEIGHT && last_x >= 0) {
        data->point.x = last_x;
    }
    else {
        data->point.x = 0;
    }
    if (last_y <= HX8357_TFTWIDTH && last_y >= 0) {
        data->point.y = last_y;
    }
    else {
        data->point.y = 0;
    }
    // ESP_LOGD(TAG, "X=%i Y=%i", data->point.x, data->point.y);
}
