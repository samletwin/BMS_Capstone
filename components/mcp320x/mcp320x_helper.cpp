#include "mcp320x_helper.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "lvgl_spi_conf.h"
#include "mcp320x.h"

#define TAG "MCP320X_HELPER"

static spi_device_handle_t mcp320x_spi_handle;
static MCP3202 mcp3202_adc(VREF_MV, CS_PIN);

void mcp320x_init() {
    /* Check if SPI Bus is already initialized */
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = DISP_SPI_MISO,
        .miso_io_num = DISP_SPI_MOSI,
        .sclk_io_num = DISP_SPI_CLK,
        .quadwp_io_num = DISP_SPI_IO2,
        .quadhd_io_num = DISP_SPI_IO3,
        .max_transfer_sz = SPI_BUS_MAX_TRANSFER_SZ
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
        ESP_LOGE(TAG, "Error initialized SPI Bus, esp_err_t of %d", ret);
    }

    /* Configure SPI Device */
    spi_device_interface_config_t dev_cfg = {0};
    dev_cfg.clock_speed_hz = 1 * 1000 * 1600;  // Clock out at 1.6 MHz
    dev_cfg.mode = 0;
    dev_cfg.spics_io_num = CS_PIN;
    dev_cfg.queue_size = 1;

    ESP_ERROR_CHECK(spi_bus_add_device(TFT_SPI_HOST, &dev_cfg, &mcp320x_spi_handle));

    mcp3202_adc = MCP3202(VREF_MV, CS_PIN, mcp320x_spi_handle);
}

uint16_t mcp320x_readBattVoltage_analog_mV(bool debug) {
    uint16_t digital = mcp3202_adc.read(BATT_VOLTAGE_CHANNEL);
    uint16_t raw = mcp3202_adc.toAnalog(digital);
    uint16_t batteryVoltage_mV = raw*BATT_VOLTAGE_SCALE;
    if (true == debug) {
        ESP_LOGI(TAG, "Battery Voltage Reading - Digital: %u, Raw mV: %u, Scaled mV: %u", digital, raw, batteryVoltage_mV);
    }
    return batteryVoltage_mV;
}

uint16_t mcp320x_readBattCurrent_analog_mA(bool debug) {
    uint16_t digital = mcp3202_adc.read(BATT_CURRENT_CHANNEL);
    uint16_t raw = mcp3202_adc.toAnalog(digital);
    uint16_t batteryCurrent_mA = raw*CURRENT_SCALE;
    if (true == debug) {
        ESP_LOGI(TAG, "Battery Current Reading - Digital: %u, Raw mV: %u, Scaled mA: %u", digital, raw, batteryCurrent_mA);
    }
    return batteryCurrent_mA;
}

