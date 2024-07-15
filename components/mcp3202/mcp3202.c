#include "mcp3202_cfg.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "lvgl_spi_conf.h"
#include "driver/gpio.h"

#define TAG "MCP3202"

static spi_device_handle_t mcp3202_spi_handle;


esp_err_t mcp3202_init() {
    /* Check if SPI Bus is already initialized */
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = DISP_SPI_MISO,
        .miso_io_num = DISP_SPI_MOSI,
        .sclk_io_num = DISP_SPI_CLK,
        .quadwp_io_num = DISP_SPI_IO2,
        .quadhd_io_num = DISP_SPI_IO3,
        .max_transfer_sz = SPI_BUS_MAX_TRANSFER_SZ
    };
    
    gpio_set_direction(MCP3202_CS_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(MCP3202_CS_PIN, 1);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1600,  // Clock out at 1.6 MHz
        .mode = 0,
        .spics_io_num = MCP3202_CS_PIN,
        .queue_size = 1,
    };

    esp_err_t ret = spi_bus_initialize(TFT_SPI_HOST, &bus_cfg, 0);
    if (ESP_ERR_INVALID_ARG == ret) {
        ESP_LOGI(TAG, "Attempted to initialize SPI Bus, already initialized.");
    }
    else if (ESP_OK == ret) {
        ESP_LOGI(TAG, "Configuring SPI host %d", TFT_SPI_HOST);
        ESP_LOGI(TAG, "MISO pin: %d, MOSI pin: %d, SCLK pin: %d, IO2/WP pin: %d, IO3/HD pin: %d",
            DISP_SPI_MISO, DISP_SPI_MOSI, DISP_SPI_CLK, DISP_SPI_IO2, DISP_SPI_IO3);

        ESP_LOGI(TAG, "Max transfer size: %d (bytes)", SPI_BUS_MAX_TRANSFER_SZ);
    }
    else {
        ESP_LOGE(TAG, "Error initialized SPI Bus, esp_err_t of %s", esp_err_to_name(ret));
    }

    ret = spi_bus_add_device(TFT_SPI_HOST, &devcfg, &mcp3202_spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return ret;
    }


    return ESP_OK;
}

esp_err_t mcp3202_readDigital(mcp3202_channel_t channel, mcp3202_mode_t mode, int16_t *result_digital) {
    uint8_t tx_data[3] = { 0 };
    uint8_t rx_data[3] = { 0 };

    // MCP3202 command bits: start bit, mode (single-ended/differential), and channel selection
    tx_data[0] = 0x01;  // Start bit
    tx_data[1] = (mode << 7) | (channel << 6);

    spi_transaction_t t = {
        .length = 3 * 8,  // Total transaction length in bits
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
    };

    esp_err_t ret = spi_device_transmit(mcp3202_spi_handle, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to transmit SPI transaction: %s", esp_err_to_name(ret));
        return ret;
    }

    // The result is a 12-bit value, combining the lower 8 bits from rx_data[1] and the upper 4 bits from rx_data[2]
    int16_t digital_value = ((rx_data[1] & 0x0F) << 8) | rx_data[2];
    if (NULL != result_digital) {
        *result_digital = digital_value;
    }

    return ESP_OK;
}

int16_t mcp3202_digitalToAnalog(int16_t digital_value) {
    return ((digital_value * MCP3202_VREF_MV) / 4095);
}

int16_t mcp3202_readBatteryCurrent_mA_si16(bool debug) {
    int16_t result_digital, result_mv, result_scaled_ma;
    ESP_ERROR_CHECK(mcp3202_readDigital(BATT_CURRENT_CHANNEL, MCP3202_MODE_SINGLE_ENDED, &result_digital));
    result_mv = mcp3202_digitalToAnalog(result_digital);
    result_scaled_ma = (result_mv-2500)*BATT_CURRENT_SCALE;
    if (true == debug) {
        ESP_LOGI(TAG, "Battery Current Reading - Digital: %d, Analog: %d, Scaled: %d", result_digital, result_mv, result_scaled_ma);
    }
    return result_scaled_ma;
}

uint16_t mcp3202_readBatteryVoltage_mA_ui16(bool debug) {
    int16_t result_digital, result_mv; 
    uint16_t result_scaled_mv;
    ESP_ERROR_CHECK(mcp3202_readDigital(BATT_VOLTAGE_CHANNEL, MCP3202_MODE_SINGLE_ENDED, &result_digital));
    result_mv = mcp3202_digitalToAnalog(result_digital);
    result_scaled_mv = (uint16_t)(result_mv*BATT_VOLTAGE_SCALE);
    if (true == debug) {
        ESP_LOGI(TAG, "Battery Voltage Reading - Digital: %d, Analog: %d, Scaled: %d", result_digital, result_mv, result_scaled_mv);
    }
    return result_scaled_mv;
}