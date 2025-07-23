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
	eo_keymancer/src/autopress/autopress.cpp
	eo_keymancer/src/logger/logger.cpp
 "eo_keymancer/src/input/input_hash.h" "eo_keymancer/src/input/input.cpp" "eo_keymancer/src/utils/constants.h" "eo_keymancer/src/input/input_utils.h")

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/config)
configure_file(
    ${CMAKE_SOURCE_DIR}/eo_keymancer/config/remap.cfg
    ${CMAKE_SOURCE_DIR}/bin/config/remap.cfg
    COPYONLY
)