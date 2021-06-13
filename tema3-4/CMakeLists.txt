cmake_minimum_required (VERSION 2.8.11)
project (tema3-4)
include_directories(${YOUR_DIRECTORY})
if(WIN32)
    link_directories("${PROJECT_SOURCE_DIR}/platforms/win")
endif()

include_directories("${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include")