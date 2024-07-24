#include <stdint.h>
#include <stddef.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <driver/i2c.h>
#include "i2c_helper.h"
#include "i2c_helper_cfg.h"

static const char* TAG = "I2C_HELPER";
static bool i2c_is_init = false;
static SemaphoreHandle_t i2c_mutex[I2C_NUM_MAX] = { NULL, NULL };
static const uint8_t ACK_CHECK_EN = 1;

static void i2c_send_address(i2c_cmd_handle_t cmd, uint16_t addr, i2c_rw_t rw) {
    i2c_master_write_byte(cmd, (addr << 1) | rw, ACK_CHECK_EN);
}

static void i2c_send_register(i2c_cmd_handle_t cmd, uint32_t reg) {
    i2c_master_write_byte(cmd, reg & 0xFF, ACK_CHECK_EN);
}

bool i2c_get_init_status(void) {
    return i2c_is_init;
}

esp_err_t i2c_init(i2c_port_t port) {
    if (port != I2C_NUM_0) {
        ESP_LOGE(TAG, "Invalid I2C port: %d", port);
        return ESP_FAIL;
    }

    if (i2c_mutex[port] == NULL) {
        ESP_LOGI(TAG, "Initializing I2C port %d", port);
        i2c_mutex[port] = xSemaphoreCreateMutex();

        i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = I2C_SDA_PIN,
            .scl_io_num = I2C_SCL_PIN,
            .sda_pullup_en = I2C_SDA_PULLUP,
            .scl_pullup_en = I2C_SCL_PULLUP,
            .master.clk_speed = I2C_FREQ_HZ,
        };

        esp_err_t ret = i2c_param_config(port, &conf);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure I2C parameters");
            return ret;
        }

        ret = i2c_driver_install(port, conf.mode, 0, 0, 0);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to install I2C driver");
            return ret;
        }
    }
    i2c_is_init = true;
    return ESP_OK;
}

esp_err_t i2c_read(i2c_port_t port, uint16_t addr, uint32_t reg, uint8_t *buffer, uint16_t size) {
    if (false == i2c_is_init) {
        ESP_LOGE(TAG, "Unable to read i2c because i2c init has not been called!");
        return ESP_FAIL;
    }

    if (port != I2C_NUM_0) {
        ESP_LOGE(TAG, "Invalid I2C port: %d", port);
        return ESP_FAIL;
    }

    if (xSemaphoreTake(i2c_mutex[port], portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take I2C mutex");
        return ESP_FAIL;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_send_address(cmd, addr, I2C_MASTER_WRITE);
    i2c_send_register(cmd, reg);
    i2c_master_start(cmd);
    i2c_send_address(cmd, addr, I2C_MASTER_READ);
    i2c_master_read(cmd, buffer, size, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    xSemaphoreGive(i2c_mutex[port]);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C read failed");
    }

    return ret;
}

esp_err_t i2c_write(i2c_port_t port, uint16_t addr, uint32_t reg, const uint8_t *buffer, uint16_t size) {
    if (false == i2c_is_init) {
        ESP_LOGE(TAG, "Unable to read i2c because i2c init has not been called!");
        return ESP_FAIL;
    }

    if (port != I2C_NUM_0) {
        ESP_LOGE(TAG, "Invalid I2C port: %d", port);
        return ESP_FAIL;
    }

    if (xSemaphoreTake(i2c_mutex[port], portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to take I2C mutex");
        return ESP_FAIL;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_send_address(cmd, addr, I2C_MASTER_WRITE);
    i2c_send_register(cmd, reg);
    i2c_master_write(cmd, buffer, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    xSemaphoreGive(i2c_mutex[port]);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write failed");
    }

    return ret;
}
