include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(utils_src
	src/basic_task.c
	src/file_utils.c
	src/i2c_ctrl.c
	src/image_file.c
	src/image_split.c
	src/net_utils.c
	src/packet_buffer.c
	src/real_time_fifo.c
	src/shm_mutex.c
	src/stats.c
	src/time_utils.c
	src/mmap_file.c
	src/shm_rw_lock.c
	src/crc32.c
)

add_library(vision_utils SHARED
	${utils_src}
)

###add soft link from utils/include to include/utils###
add_custom_command(
TARGET vision_utils POST_BUILD
COMMAND rm -rf ${PROJECT_ROOT_PATH}/include/utils
COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/include ${PROJECT_ROOT_PATH}/include/utils
)
