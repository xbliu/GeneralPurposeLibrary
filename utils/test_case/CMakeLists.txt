

#################googletest########################
set(GOOGLETEST_INSTALL_DIR ${PROJ_STAGING_ROOT_PATH})
set(GOOGLETEST_BUILD_DIR ${PROJ_STAGING_DIR}/build/googletest_1.11)
add_custom_target(build_googletest  ALL
	COMMAND mkdir -p ${GOOGLETEST_BUILD_DIR}
	COMMAND cd ${GOOGLETEST_BUILD_DIR} &&
	${CMAKE_COMMAND} -DGOOGLETEST_VERSION=1.11 -DBUILD_GMOCK=OFF -DCMAKE_TOOLCHAIN_FILE=${PROJECT_ROOT_PATH}/common_toolchain.cmake -DCMAKE_INSTALL_PREFIX=${GOOGLETEST_INSTALL_DIR} ${PROJECT_ROOT_PATH}/test_case/googletest-release-1.11.0 && 
	${CMAKE_MAKE_PROGRAM} -j4 && ${CMAKE_MAKE_PROGRAM} install
	WORKING_DIRECTORY ${PROJ_STAGING_DIR}/build
	COMMENT "build googletest")



