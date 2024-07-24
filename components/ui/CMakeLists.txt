file(GLOB SOURCES *.c)
set(INCLUDE_DIRS "include")


list(APPEND SOURCES "ui_animations.cpp")
list(APPEND SOURCES "ui_screen.cpp")
list(APPEND SOURCES "ui_manager.cpp")
list(APPEND SOURCES "ui_main.cpp")
                
idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${INCLUDE_DIRS}
                        REQUIRES lvgl freertos)