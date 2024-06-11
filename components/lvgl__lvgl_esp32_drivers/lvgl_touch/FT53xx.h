/**
 * @file FT53xx.h
 * @author Sam Letwin (letwins@uwindsor.ca)
 * @brief 
 * @version 0.1
 * @date 2024-06-10
 * 
 * 
 */

#ifndef __FT53XX_H
#define __FT53XX_H

#include <stdint.h>
#include <stdbool.h>
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FT53XX_DEFAULT_ADDR 0x38 //!< I2C address
#define FT53XX_REG_VENDID 0xA3   //!< FocalTech's panel ID
#define FT53XX_REG_FIRMVERS 0xA6 //!< Firmware version
#define FT53XX_REG_CHIPID 0xA8   //!< Chip selecting

#define FT53XX_VENDID 0x11 //!< FocalTech's panel ID
#define FT5336_CHIPID 0x79 //!< Chip selecting

#define FT53XX_REG_NUMTOUCHES 0x02 //!< Number of touch points

#define FT5336_REG_THRESHGROUP 0x80 //!< Threshold for touch detection

#define FT5336_TD_STATUS 0x02 ///<! Touch status
#define FT5336_GEST_ID 0x01   ///<! Detected Gesture ID
#define FT5336_TOUCH1_XH 0x03 ///<! High byte of X touch #1
#define FT5336_TOUCH1_XL 0x04 ///<! Low byte of X touch #1
#define FT5336_TOUCH1_YH 0x05 ///<! High byte of Y touch #1
#define FT5336_TOUCH1_YL 0x06 ///<! Low byte of Y touch #1

#define FT53XX_NUM_X 0x33 //!< Touch X position
#define FT53XX_NUM_Y 0x34 //!< Touch Y position

#define FT53XX_REG_MODE 0x00        //!< Device mode, either WORKING or FACTORY
#define FT53XX_REG_CALIBRATE 0x02   //!< Calibrate mode
#define FT53XX_REG_WORKMODE 0x00    //!< Work mode
#define FT53XX_REG_FACTORYMODE 0x40 //!< Factory mode
#define FT53XX_REG_THRESHHOLD 0x80  //!< Threshold for touch detection
#define FT53XX_REG_POINTRATE 0x88   //!< Point rate

// calibrated for Adafruit 3.5" ctp screen
#define FT62XX_DEFAULT_THRESHOLD 128 //!< Default threshold for touch detection

typedef struct {
    bool inited;
} ft53xx_status_t;

/**
  * @brief  Initialize for FT53xx communication via I2C
  * @param  dev_addr: Device address on communication Bus (I2C slave address of FT6X36).
  * @retval None
  */
void ft53xx_init(uint16_t dev_addr);

uint8_t ft53xx_get_gesture_id();

/**
  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
  * @param  drv:
  * @param  data: Store data here
  * @retval Always false
  */
bool ft53xx_read(lv_indev_drv_t *drv, lv_indev_data_t *data);


#ifdef __cplusplus
}
#endif

#endif // __FT53XX_H
