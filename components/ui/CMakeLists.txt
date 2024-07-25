file(GLOB SOURCES *.c)
set(INCLUDE_DIRS "include" "widgets" "colour_swap")


list(APPEND SOURCES "ui_animation.c")
list(APPEND SOURCES "ui_screen.c")
list(APPEND SOURCES "ui_manager.c")
list(APPEND SOURCES "ui_main.c")
list(APPEND SOURCES "widgets/battery_widget.c")
list(APPEND SOURCES "colour_swap/colour_swap.c")
                
idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${INCLUDE_DIRS}
                        REQUIRES lvgl freertos)