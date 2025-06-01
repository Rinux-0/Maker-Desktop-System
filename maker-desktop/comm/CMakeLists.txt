add_subdirectory_if_exist(format)

if(DEFINED CONFIG_COMM_I2C)
	add_subdirectory_if_exist(i2c)
endif()
if(DEFINED CONFIG_COMM_UART)
	add_subdirectory_if_exist(uart)
endif()
if(DEFINED CONFIG_COMM_SLE)
	add_subdirectory_if_exist(sle)
endif()
if(DEFINED CONFIG_COMM_WIFI)
	add_subdirectory_if_exist(wifi)
endif()



file(GLOB_RECURSE SRC_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}/comm.c
)
set(HEAD_DIR_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}
)



set(SOURCES ${SOURCES}
	${SRC_CURRENT}
	PARENT_SCOPE
)
set(PUBLIC_HEADER ${PUBLIC_HEADER}
	${HEAD_DIR_CURRENT}
	PARENT_SCOPE
)
