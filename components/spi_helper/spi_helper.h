#ifndef SPI_HELPER_H 
#define SPI_HELPER_H 

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/spi_master.h"

typedef enum _spi_helper_send_flag_t {
    SPI_SEND_QUEUED        = 0x00000000,
    SPI_SEND_POLLING       = 0x00000001,
    SPI_SEND_SYNCHRONOUS   = 0x00000002,
    SPI_SIGNAL_FLUSH       = 0x00000004,
    SPI_RECEIVE            = 0x00000008,
    SPI_CMD_8              = 0x00000010, /* Reserved */
    SPI_CMD_16             = 0x00000020, /* Reserved */
    SPI_ADDRESS_8          = 0x00000040, 
    SPI_ADDRESS_16         = 0x00000080, 
    SPI_ADDRESS_24         = 0x00000100, 
    SPI_ADDRESS_32         = 0x00000200, 
    SPI_MODE_DIO           = 0x00000400, 
    SPI_MODE_QIO           = 0x00000800, 
    SPI_MODE_DIOQIO_ADDR   = 0x00001000, 
	SPI_VARIABLE_DUMMY	   = 0x00002000
} spi_helper_send_flag_t;

/// @brief just for initializing the transaction pool
/// @param  
void spi_helper_init(void);
void spi_helper_transaction(const uint8_t *data, size_t length, spi_helper_send_flag_t flags, uint8_t *out, uint64_t addr, uint8_t dummy_bits, spi_device_handle_t spi);
void spi_helper_wait_for_pending_transactions(spi_device_handle_t spi);

static inline void spi_helper_send_data(uint8_t *data, size_t length, spi_device_handle_t spi) {
    spi_helper_transaction(data, length, SPI_SEND_POLLING, NULL, 0, 0, spi);
}

static inline void spi_helper_send_colors(uint8_t *data, size_t length, spi_device_handle_t spi) {
    spi_helper_transaction(data, length, SPI_SEND_QUEUED | SPI_SIGNAL_FLUSH, NULL, 0, 0, spi);
}

#ifdef __cplusplus
} /* extern C */
#endif

#endif /* SPI_HELPER_H */