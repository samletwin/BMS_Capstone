
file(GLOB SOURCES *.c)
set(INCLUDE_DIRS .)


list(APPEND SOURCES "main.cpp")
                
idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${INCLUDE_DIRS}
                        REQUIRES lvgl bms spi_helper lvgl_driver freertos driver esp_timer ui
                        lvgl_example daly_bms_arduino)