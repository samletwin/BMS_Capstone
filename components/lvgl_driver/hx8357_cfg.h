#ifndef HX8357_CFG_H
#define HX8357_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#define HX8357_DC             -1
#define HX8357_RST            -1
#define HX8357_USE_RST        1
#define HX8357_INVERT_COLORS  0

/* DISP_BUF_SIZE value doesn't have an special meaning, but it's the size
 * of the buffer(s) passed to LVGL as display buffers. The default values used
 * were the values working for the contributor of the display controller.
 *
 * As LVGL supports partial display updates the DISP_BUF_SIZE doesn't
 * necessarily need to be equal to the display size.
 *
 * When using RGB displays the display buffer size will also depends on the
 * color format being used, for RGB565 each pixel needs 2 bytes.
 * When using the mono theme, the display pixels can be represented in one bit,
 * so the buffer size can be divided by 8, e.g. see SSD1306 display size. */
#define HX8357_BUF_SIZE       480 * 40

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HX8357_CFG_H */