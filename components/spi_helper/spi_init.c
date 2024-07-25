#include <stdio.h>
#include "spi_init.h"
#include "spi_helper_cfg.h"
#include "spi_helper.h"
#include "esp_err.h"
#include "hx8357_cfg.h"
#include "hx8357.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#define TAG "SPI_INIT"

spi_device_handle_t hx8357_spi_handle;
spi_device_handle_t sd_spi_handle;

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

esp_err_t spi_sd_init() {
    ESP_LOGI(TAG, "Adding SPI device SD Card");

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_CLOCK_SPEED,  // You might want to adjust this for SD card
        .mode = 0,
        .spics_io_num = SD_CARD_CS_PIN,
        .queue_size = SPI_TRANSACTION_POOL_SIZE,
        .flags = SPI_DEVICE_NO_DUMMY,
    };

    esp_err_t ret = spi_bus_add_device(SPI_CFG_HOST, &devcfg, &sd_spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SD card SPI device");
        return ret;
    }

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI_CFG_HOST;
    host.max_freq_khz = SPI_CLOCK_SPEED / 1000;

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_CARD_CS_PIN;
    slot_config.host_id = host.slot;

    sdmmc_card_t* card;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    const char mount_point[] = "/sdcard";
    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    return ESP_OK;
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
    if (SPI_INIT_DISPLAY_ONLY == initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for display only");
    else if (SPI_INIT_SD_LOG_ONLY == initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for SD Log only");
    else if (SPI_INIT_BOTH == initType_e)
        ESP_LOGI(TAG, "Initializing SPI master for both SD Log and Display");
    else
        ESP_LOGE(TAG, "Initializing SPI master for not defined devices");
    
    ESP_LOGI(TAG, "Configuring SPI SPI_CFG_HOST %d", SPI_CFG_HOST);

    esp_err_t ret = spi_bus_init();
    if (ret != ESP_OK) {
        return ret;
    }

    if (initType_e == SPI_INIT_DISPLAY_ONLY || initType_e == SPI_INIT_BOTH) {
        ret = spi_hx8357_init();
        if (ret != ESP_OK) {
            return ret;
        }
        hx8357_init();
    }
    
    if (initType_e == SPI_INIT_SD_LOG_ONLY || initType_e == SPI_INIT_BOTH) {
        ret = spi_sd_init();
        if (ret != ESP_OK) {
            return ret;
        }
    }

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

