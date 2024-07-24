idf_component_register(SRCS "hx8357.c" "lvgl_driver_init.c" "FT53xx.c"
                    INCLUDE_DIRS .
                    REQUIRES driver spi_helper lvgl esp_rom i2c_helper)
