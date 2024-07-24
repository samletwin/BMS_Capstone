
file(GLOB SOURCES *.c)
set(INCLUDE_DIRS .)


list(APPEND SOURCES "main.cpp")
list(APPEND SOURCES)
                
idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${INCLUDE_DIRS}
                        REQUIRES lvgl bms daly_bms_serial spi_helper lvgl_driver freertos driver esp_timer ui)