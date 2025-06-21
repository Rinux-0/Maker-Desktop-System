if(DEFINED CONFIG_TEST_KEYTEST)
	add_subdirectory_if_exist(keytest)
elseif(DEFINED CONFIG_TEST_TMP)
	add_subdirectory_if_exist(tmp)
else()
	message(FATAL_ERROR "This test can't be recognized!")
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
