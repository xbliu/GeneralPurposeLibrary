
add_compile_options(-g -w)
set(PROJECT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#############set output directory: PROJECT_ROOT_PATH or PROJ_OUTPUT_PATH#############
if(NOT DEFINED $ENV{PROJ_OUTPUT_DIR})
	set(PROJ_STAGING_DIR ${PROJECT_ROOT_PATH}/out/staging)
	set(PROJ_INSTALL_DIR ${PROJECT_ROOT_PATH}/out/install)
else()
	set(PROJ_STAGING_DIR $ENV{PROJ_OUTPUT_PATH}/out/staging)
	set(PROJ_INSTALL_DIR $ENV{PROJ_OUTPUT_PATH}/out/install)
endif()

if(NOT DEFINED $ENV{PROJ_PLATFORM})
	set(ENV{PROJ_PLATFORM} x86)
endif()

set(PROJ_LIB_PATH ${PROJ_STAGING_DIR}/$ENV{PROJ_PLATFORM}/lib)
set(PROJ_BIN_PATH ${PROJ_STAGING_DIR}/$ENV{PROJ_PLATFORM}/bin)
set(PROJ_BUILD_PATH ${PROJ_STAGING_DIR}/build)
set(PROJ_STAGING_ROOT_PATH ${PROJ_STAGING_DIR}/$ENV{PROJ_PLATFORM})
set(LIBRARY_OUTPUT_PATH ${PROJ_LIB_PATH})
set(EXECUTABLE_OUTPUT_PATH ${PROJ_BIN_PATH})

#############set cross compile#############
set(CMAKE_C_COMPILER $ENV{CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER $ENV{CROSS_COMPILE}g++)

#############do not remove it in android#############
set(CMAKE_SKIP_BUILD_RPATH TRUE)

#############create build dir for all target#############
execute_process(
	COMMAND mkdir -p ${PROJ_BUILD_PATH}
)

#############output debug message#############
message("PROJ_LIB_PATH:${PROJ_LIB_PATH}  PROJ_BIN_PATH:${PROJ_BIN_PATH} PROJ_BUILD_PATH:${PROJ_BUILD_PATH}")
message("CMAKE_C_COMPILER:${CMAKE_C_COMPILER} CMAKE_CXX_COMPILER:${CMAKE_CXX_COMPILER}")
