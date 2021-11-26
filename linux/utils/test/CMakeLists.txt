add_compile_options(-Werror)

set(src_root_path ${CMAKE_CURRENT_SOURCE_DIR}/../utils)

set(basic_task_test_src
	${src_root_path}/basic_task.c
	basic_task_test.c
)
add_executable(basic_task_test 
	${basic_task_test_src}
)
target_link_libraries(basic_task_test 
pthread
m
libstdc++.so
)

set(memmap_test_src
	${src_root_path}/memmap.c
	memmap_test.c
)
add_executable(memmap_test 
	${memmap_test_src}
)

set(stats_test_src
	${src_root_path}/stats.c
	stats_test.c
)
add_executable(stats_test 
	${stats_test_src}
)

set(sc132_xchip_reg_src
	${src_root_path}/i2c_ctrl.c
	sc132_xchip_reg.c
)
add_executable(sc132_xchip_reg 
	${sc132_xchip_reg_src}
)


