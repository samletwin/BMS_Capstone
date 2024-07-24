
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

#define TAG "SPI_HELPER"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "spi_helper.h"
#include "spi_helper_cfg.h"

static QueueHandle_t TransactionPool = NULL;

void spi_helper_init(void) {
    /* create the transaction pool and fill it with ptrs to spi_transaction_ext_t to reuse */
	if(TransactionPool == NULL) {
		TransactionPool = xQueueCreate(SPI_TRANSACTION_POOL_SIZE, sizeof(spi_transaction_ext_t*));
		assert(TransactionPool != NULL);
		for (size_t i = 0; i < SPI_TRANSACTION_POOL_SIZE; i++)
		{
			spi_transaction_ext_t* pTransaction = (spi_transaction_ext_t*)heap_caps_malloc(sizeof(spi_transaction_ext_t), MALLOC_CAP_DMA);
			assert(pTransaction != NULL);
			memset(pTransaction, 0, sizeof(spi_transaction_ext_t));
			xQueueSend(TransactionPool, &pTransaction, portMAX_DELAY);
		}
	}
}

void spi_helper_transaction(const uint8_t *data, size_t length, spi_helper_send_flag_t flags, uint8_t *out, uint64_t addr, uint8_t dummy_bits, spi_device_handle_t spi)
{
    if (0 == length) {
        return;
    }

    size_t max_transfer_size = SPI_MAX_TRANSFER_SIZE; // Define this as per your hardware capability
    size_t remaining_length = length;
    const uint8_t *current_data = data;

    while (remaining_length > 0) {
        size_t chunk_size = (remaining_length > max_transfer_size) ? max_transfer_size : remaining_length;

        spi_transaction_ext_t t = {0};

        /* transaction length is in bits */
        t.base.length = chunk_size * 8;

        if (chunk_size <= 4 && current_data != NULL) {
            t.base.flags = SPI_TRANS_USE_TXDATA;
            memcpy(t.base.tx_data, current_data, chunk_size);
        } else {
            t.base.tx_buffer = current_data;
        }

        if (flags & SPI_RECEIVE) {
            assert(out != NULL && (flags & (SPI_SEND_POLLING | SPI_SEND_SYNCHRONOUS)));
            t.base.rx_buffer = out;

#if defined(SPI_HELPER_HALF_DUPLEX)
            t.base.rxlength = t.base.length;
            t.base.length = 0; /* no MOSI phase in half-duplex reads */
#else
            t.base.rxlength = 0; /* in full-duplex mode, zero means same as tx length */
#endif
        }

        if (flags & SPI_ADDRESS_8) {
            t.address_bits = 8;
        } else if (flags & SPI_ADDRESS_16) {
            t.address_bits = 16;
        } else if (flags & SPI_ADDRESS_24) {
            t.address_bits = 24;
        } else if (flags & SPI_ADDRESS_32) {
            t.address_bits = 32;
        }
        if (t.address_bits) {
            t.base.addr = addr;
            t.base.flags |= SPI_TRANS_VARIABLE_ADDR;
        }

#if defined(SPI_HELPER_HALF_DUPLEX)
        if (flags & SPI_MODE_DIO) {
            t.base.flags |= SPI_TRANS_MODE_DIO;
        } else if (flags & SPI_MODE_QIO) {
            t.base.flags |= SPI_TRANS_MODE_QIO;
        }

        if (flags & SPI_MODE_DIOQIO_ADDR) {
            t.base.flags |= SPI_TRANS_MODE_DIOQIO_ADDR;
        }

        if ((flags & SPI_VARIABLE_DUMMY) && dummy_bits) {
            t.dummy_bits = dummy_bits;
            t.base.flags |= SPI_TRANS_VARIABLE_DUMMY;
        }
#endif

        /* Save flags for pre/post transaction processing */
        t.base.user = (void *)flags;

        /* Poll/Complete/Queue transaction */
        if (flags & SPI_SEND_POLLING) {
            spi_helper_wait_for_pending_transactions(spi); /* before polling, all previous pending transactions need to be serviced */
            spi_device_polling_transmit(spi, (spi_transaction_t *)&t);
        } else if (flags & SPI_SEND_SYNCHRONOUS) {
            spi_helper_wait_for_pending_transactions(spi); /* before synchronous queueing, all previous pending transactions need to be serviced */
            spi_device_transmit(spi, (spi_transaction_t *)&t);
        } else {

            /* if necessary, ensure we can queue new transactions by servicing some previous transactions */
            if (uxQueueMessagesWaiting(TransactionPool) == 0) {
                spi_transaction_t *presult;
                while (uxQueueMessagesWaiting(TransactionPool) < SPI_TRANSACTION_POOL_RESERVE) {
                    if (spi_device_get_trans_result(spi, &presult, 1) == ESP_OK) {
                        xQueueSend(TransactionPool, &presult, portMAX_DELAY); /* back to the pool to be reused */
                    }
                }
            }

            spi_transaction_ext_t *pTransaction = NULL;
            xQueueReceive(TransactionPool, &pTransaction, portMAX_DELAY);
            memcpy(pTransaction, &t, sizeof(t));
            if (spi_device_queue_trans(spi, (spi_transaction_t *)pTransaction, portMAX_DELAY) != ESP_OK) {
                xQueueSend(TransactionPool, &pTransaction, portMAX_DELAY); /* send failed transaction back to the pool to be reused */
            }
        }

        remaining_length -= chunk_size;
        current_data += chunk_size;
    }
}

void spi_helper_wait_for_pending_transactions(spi_device_handle_t spi)
{
    spi_transaction_t *presult;

	while(uxQueueMessagesWaiting(TransactionPool) < SPI_TRANSACTION_POOL_SIZE) {	/* service until the transaction reuse pool is full again */
        if (spi_device_get_trans_result(spi, &presult, 1) == ESP_OK) {
			xQueueSend(TransactionPool, &presult, portMAX_DELAY);
        }
    }
}