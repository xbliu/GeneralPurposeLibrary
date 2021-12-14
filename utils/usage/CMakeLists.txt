set(USAGE_SRC_DIR ${PROJECT_ROOT_PATH}/src)

############Use Cases############
set(uc_basic_task_src
	${USAGE_SRC_DIR}/pthread/basic_task.c
	basic_task_usage.c
)
add_executable(uc_basic_task ${uc_basic_task_src})
target_link_libraries(uc_basic_task pthread)


