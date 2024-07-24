idf_component_register(SRCS "spi_init.c" "spi_helper.c"
                    INCLUDE_DIRS . 
                    REQUIRES driver lvgl_driver freertos)
