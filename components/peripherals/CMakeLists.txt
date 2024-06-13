
file(GLOB SOURCES *.c)
set(INCLUDE_DIRS . include)

list(APPEND SOURCES "adc.c")
list(APPEND SOURCES "gpio.c")
                
idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${INCLUDE_DIRS}
                        REQUIRES "esp_adc_cal")