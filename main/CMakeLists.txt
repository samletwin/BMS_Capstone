
file(GLOB SOURCES *.c)
set(INCLUDE_DIRS .)

list(APPEND SOURCES "ui/screens.c")
list(APPEND SOURCES "ui/ui.c")
list(APPEND SOURCES "ui/styles.c")
list(APPEND SOURCES "main_singleCell.cpp")
list(APPEND SOURCES "main_dalyBms.cpp")
list(APPEND SOURCES "global_vars.c")
                
idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${INCLUDE_DIRS}
                        REQUIRES lvgl peripherals bms timing daly_bms_serial)