cmake_minimum_required(VERSION 3.17)
project(eo_keymancer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)

add_executable(eo_keymancer
    eo_keymancer/src/main.cpp
    eo_keymancer/src/remapper/remapper.cpp
    eo_keymancer/src/keyhook/keyhook.cpp
)