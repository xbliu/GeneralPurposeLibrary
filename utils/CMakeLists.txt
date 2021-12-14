#######################################################################
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_SOURCE_DIR}/common_toolchain.cmake)
project(gnpl)


############################include dir###########################################
include_directories(${PROJECT_ROOT_PATH}/include)
include_directories(${PROJECT_ROOT_PATH}/include/pthread)


############################sub dir###########################################
add_subdirectory(src)

option(TEST_CASE "option for test case" OFF)
if(${TEST_CASE} STREQUAL "ON")
	add_subdirectory(test_case)
endif()

option(USAGE "option for usage" ON)
if(${USAGE} STREQUAL "ON")
	add_subdirectory(usage)
endif()
