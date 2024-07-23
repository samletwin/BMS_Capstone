#ifndef SPI_INIT_H
#define SPI_INIT_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SPI_INIT_DISPLAY_ONLY=0,
    SPI_INIT_SD_LOG_ONLY=1,
    SPI_INIT_BOTH=2,
} spi_init_type;

/// @brief Function that handles initializing SPI bus, as well as adding HX8357 Display and SD Card to the bus.
/// @return  
esp_err_t spi_init(spi_init_type initType_e);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SPI_INIT_H */