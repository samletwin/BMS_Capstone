# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Sam/esp/v4.4.7/esp-idf/components/bootloader/subproject"
  "C:/Users/Sam/Desktop/capstone/singleCell_espidf/build/bootloader"
  "C:/Users/Sam/Desktop/capstone/singleCell_espidf/build/bootloader-prefix"
  "C:/Users/Sam/Desktop/capstone/singleCell_espidf/build/bootloader-prefix/tmp"
  "C:/Users/Sam/Desktop/capstone/singleCell_espidf/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/Sam/Desktop/capstone/singleCell_espidf/build/bootloader-prefix/src"
  "C:/Users/Sam/Desktop/capstone/singleCell_espidf/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Sam/Desktop/capstone/singleCell_espidf/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
