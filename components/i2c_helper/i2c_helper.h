#ifndef I2C_HELPER_H
#define I2C_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <driver/i2c.h>

#define I2C_ADDR_10 ( 1 << 15 )
#define I2C_REG_16  ( 1 << 31 )
#define I2C_NO_REG  ( 1 << 30 )

bool i2c_get_init_status(void);
esp_err_t i2c_init(i2c_port_t port);
esp_err_t i2c_read(i2c_port_t port, uint16_t addr, uint32_t reg, uint8_t *buffer, uint16_t size);
esp_err_t i2c_write(i2c_port_t port, uint16_t addr, uint32_t reg, const uint8_t *buffer, uint16_t size);

#ifdef __cplusplus
}
#endif
#endif // I2C_HELPER_H
