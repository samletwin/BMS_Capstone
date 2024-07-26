
                    
file(GLOB SOURCES *.c)
set(INCLUDE_DIRS . include)

list(APPEND SOURCES "soh.cpp")
list(APPEND SOURCES "soc.cpp")
                
idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${INCLUDE_DIRS}
                        REQUIRES esp-dsp arduino-esp32)