#ifndef _SPI_HELPER_CFG_H
#define _SPI_HELPER_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hx8357_cfg.h"
#include "hal/spi_types.h"


#define DISP_CS_PIN         2
#define SD_CARD_CS_PIN      3

#define SPI_MOSI_PIN        8
#define SPI_MISO_PIN        9
#define SPI_CLK_PIN         20
#define SPI_QUADWP_PIN      -1
#define SPI_QUADHD_PIN      -1

#define SPI_CLOCK_SPEED     (26*1000*1000)

#define SPI_MAX_TRANSFER_SIZE   HX8357_BUF_SIZE

#define SPI_CFG_HOST            SPI2_HOST

/* Uncomment below is use SPI in Half Duplex mode */
#define SPI_HELPER_HALF_DUPLEX

#define SPI_TRANSACTION_POOL_SIZE 50	/* maximum number of DMA transactions simultaneously in-flight */

/* DMA Transactions to reserve before queueing additional DMA transactions. A 1/10th seems to be a good balance. Too many (or all) and it will increase latency. */
#define SPI_TRANSACTION_POOL_RESERVE_PERCENTAGE 10
#if SPI_TRANSACTION_POOL_SIZE >= SPI_TRANSACTION_POOL_RESERVE_PERCENTAGE
#define SPI_TRANSACTION_POOL_RESERVE (SPI_TRANSACTION_POOL_SIZE / SPI_TRANSACTION_POOL_RESERVE_PERCENTAGE)	
#else
#define SPI_TRANSACTION_POOL_RESERVE 1	/* defines minimum size */
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* _SPI_HELPER_CFG_H */