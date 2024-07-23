#include <stdio.h>
#include "spi_init.h"
#include "spi_helper_cfg.h"
#include "esp_err.h"
#include "hx8357_cfg.h"
#include "hx8357.h"

#define TAG "SPI_INIT"

static spi_device_handle_t hx8357_spi_handle;


static void IRAM_ATTR spi_ready (spi_transaction_t *trans);

esp_err_t spi_bus_init() {
    spi_bus_config_t buscfg = {
        .miso_io_num = SPI_MISO_PIN,
	    .mosi_io_num = SPI_MOSI_PIN,
	    .sclk_io_num = SPI_CLK_PIN,
	    .quadwp_io_num = SPI_QUADWP_PIN,
	    .quadhd_io_num = SPI_QUADHD_PIN,
        .max_transfer_sz = SPI_MAX_TRANSFER_SIZE
    };

    esp_err_t ret = spi_bus_initialize(SPI_HOST, &buscfg, 0);
    assert(ret == ESP_OK);

    return ESP_OK != ret;
}

esp_err_t spi_hx8357_init() {
    ESP_LOGI(TAG, "Adding SPI device HX8357");

    spi_device_interface_config_t devcfg={
        .clock_speed_hz = SPI_CLOCK_SPEED,
        .mode = 0,
        .spics_io_num=DISP_CS_PIN,              // CS pin
        .input_delay_ns=0,
        .queue_size=SPI_TRANSACTION_POOL_SIZE,
        .pre_cb=NULL,
        .post_cb=NULL,
#if defined(SPI_HELPER_HALF_DUPLEX)
        .flags = SPI_DEVICE_NO_DUMMY | SPI_DEVICE_HALFDUPLEX,	
#endif
    };

    esp_err_t ret = spi_bus_add_device(SPI_HOST, devcfg, &hx8357_spi_handle)
    assert(ret==ESP_OK);
    return ret;
}

esp_err_t spi_full_init(spi_init_type initType_e); {
    if (SPI_INIT_DISPLAY_ONLY==initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for display only");
    else if (SPI_INIT_SD_LOG_ONLY==initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for SD Log only");
    else if (SPI_INIT_BOTH==initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for both SD Log and Display");
    else
        ESP_LOGE(TAG, "Initializing SPI master for not defined devices");
    
    ESP_LOGI(TAG, "Configuring SPI host %d", SPI_HOST)

    spi_bus_init();
    spi_hx8357_init();
    hx8357_init();
}

static void IRAM_ATTR spi_ready(spi_transaction_t *trans)
{
    spi_helper_send_flag_t flags = (spi_helper_send_flag_t) trans->user;

    if (flags & spi_helper_SIGNAL_FLUSH) {
        lv_disp_t * disp = NULL;

        disp = _lv_refr_get_disp_refreshing();

        lv_disp_flush_ready(disp->driver);

    }

    if (spi_ready) {
        spi_ready(trans);
    }
}

