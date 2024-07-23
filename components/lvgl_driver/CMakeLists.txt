idf_component_register(SRCS "hx8357.c" "lvgl_helper.c"
                    INCLUDE_DIRS "."
                    REQUIRES spi_helper lvgl-9.1.0)
