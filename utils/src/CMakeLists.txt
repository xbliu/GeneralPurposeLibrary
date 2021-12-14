set(gnpl_src
	pthread/basic_task.c
)

add_library(gnpl SHARED
	${gnpl_src}
)
