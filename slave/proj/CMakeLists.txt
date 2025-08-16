add_subdirectory_if_exist(comm)
add_subdirectory_if_exist(util)

if(DEFINED CONFIG_DEVICE)
	add_subdirectory_if_exist(device)
elseif(DEFINED CONFIG_TEST)
	add_subdirectory_if_exist(test)
else()
	message(FATAL_ERROR "There is no device or test!")
endif()



file(GLOB SRC_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
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
