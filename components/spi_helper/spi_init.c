#include <stdio.h>
#include "spi_init.h"
#include "spi_helper_cfg.h"
#include "spi_helper.h"
#include "esp_err.h"
#include "hx8357_cfg.h"
#include "hx8357.h"
#include "driver/spi_master.h"
#include "esp_log.h"

#define TAG "SPI_INIT"

spi_device_handle_t hx8357_spi_handle;


static void IRAM_ATTR spi_ready (spi_transaction_t *trans);

esp_err_t spi_bus_init() {
    int dma_chan = 0 /* SPI_DMA_DISABLED */;

#if defined (CONFIG_IDF_TARGET_ESP32)
    assert((SPI_HOST <= SPI_CFG_HOST) && (VSPI_HOST >= SPI_CFG_HOST));
    const char *spi_names[] = {
        "SPI_HOST", "HSPI_HOST", "VSPI_HOST"
    };

    dma_chan = dma_channel;
#elif defined (CONFIG_IDF_TARGET_ESP32S2)
    assert((SPI_HOST <= SPI_CFG_HOST) && (HSPI_HOST >= SPI_CFG_HOST));
    const char *spi_names[] = {
        "SPI_HOST", "", ""
    };

    dma_chan = dma_channel;
#elif defined (CONFIG_IDF_TARGET_ESP32C3)
    assert((SPI1_HOST <= SPI_CFG_HOST) && (SPI2_HOST >= SPI_CFG_HOST));
    const char *spi_names[] = {
        "SPI1_HOST", "SPI2_HOST", "SPI3_HOST"
    };

    dma_chan = 3 /* SPI_DMA_CH_AUTO */;
#else
#error "Target chip not selected"
#endif
    
    spi_helper_init();

    spi_bus_config_t buscfg = {
        .miso_io_num = SPI_MISO_PIN,
	    .mosi_io_num = SPI_MOSI_PIN,
	    .sclk_io_num = SPI_CLK_PIN,
	    .quadwp_io_num = SPI_QUADWP_PIN,
	    .quadhd_io_num = SPI_QUADHD_PIN,
        .max_transfer_sz = SPI_MAX_TRANSFER_SIZE
    };

    esp_err_t ret = spi_bus_initialize(SPI_CFG_HOST, &buscfg, dma_chan);
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
        .post_cb=spi_ready,
#if defined(SPI_HELPER_HALF_DUPLEX)
        .flags = SPI_DEVICE_NO_DUMMY | SPI_DEVICE_HALFDUPLEX,	
#endif
    };

    esp_err_t ret = spi_bus_add_device(SPI_CFG_HOST, &devcfg, &hx8357_spi_handle);
    assert(ret==ESP_OK);
    return ret;
}

esp_err_t spi_full_init(spi_init_type initType_e) {
    if (SPI_INIT_DISPLAY_ONLY==initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for display only");
    else if (SPI_INIT_SD_LOG_ONLY==initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for SD Log only");
    else if (SPI_INIT_BOTH==initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for both SD Log and Display");
    else
        ESP_LOGE(TAG, "Initializing SPI master for not defined devices");
    
    ESP_LOGI(TAG, "Configuring SPI SPI_CFG_HOST %d", SPI_CFG_HOST);

    spi_bus_init();
    spi_hx8357_init();
    hx8357_init();

    return ESP_OK;
}

static void IRAM_ATTR spi_ready(spi_transaction_t *trans)
{
    spi_helper_send_flag_t flags = (spi_helper_send_flag_t) trans->user;

    if (flags & SPI_SIGNAL_FLUSH) {
        lv_display_t * disp = NULL;

        disp = _lv_refr_get_disp_refreshing();

        lv_disp_flush_ready(disp);

    }

    // if (spi_ready) {
    //     spi_ready(trans);
    // }
}

