add_subdirectory_if_exist(device)
# add_subdirectory_if_exist(sle)



file(GLOB_RECURSE SRC_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}/util/comm/custom/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/comm/hid/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/comm/uart/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/def/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/tool/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/demo.c
)
set(HEAD_DIR_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}/util/comm/custom
	${CMAKE_CURRENT_SOURCE_DIR}/util/comm/hid
	${CMAKE_CURRENT_SOURCE_DIR}/util/comm/sle
	${CMAKE_CURRENT_SOURCE_DIR}/util/comm/uart
	${CMAKE_CURRENT_SOURCE_DIR}/util/def
	${CMAKE_CURRENT_SOURCE_DIR}/util/tool
	${CMAKE_CURRENT_SOURCE_DIR}
)



if(DEFINED CONFIG_COMM_CUSTOM)
	file(GLOB_RECURSE SRC_COMM_CUSTOM
		${CMAKE_CURRENT_SOURCE_DIR}/comm/custom/*.c
	)
	set(SRC_CURRENT ${SRC_CURRENT}
		${SRC_COMM_CUSTOM}
	)
elseif(DEFINED CONFIG_COMM_HID)
	file(GLOB_RECURSE SRC_COMM_HID
		${CMAKE_CURRENT_SOURCE_DIR}/comm/hid/*.c
	)
	set(SRC_CURRENT ${SRC_CURRENT}
		${SRC_COMM_HID}
	)
endif()



if(DEFINED CONFIG_COMM_SLE_CLIENT)
	set(SRC_CURRENT ${SRC_CURRENT}
		${CMAKE_CURRENT_SOURCE_DIR}/util/comm/sle/client/ssle_core_client.c
		${CMAKE_CURRENT_SOURCE_DIR}/util/comm/sle/client/ssle_client.c
		${CMAKE_CURRENT_SOURCE_DIR}/util/comm/sle/ssle.c
	)
elseif(DEFINED CONFIG_COMM_SLE_SERVER)
	set(SRC_CURRENT ${SRC_CURRENT}
		${CMAKE_CURRENT_SOURCE_DIR}/util/comm/sle/server/ssle_core_server_adv.c
		${CMAKE_CURRENT_SOURCE_DIR}/util/comm/sle/server/ssle_core_server.c
		${CMAKE_CURRENT_SOURCE_DIR}/util/comm/sle/server/ssle_server.c
		${CMAKE_CURRENT_SOURCE_DIR}/util/comm/sle/ssle.c
	)
endif()



set(SOURCES ${SOURCES}
	${SRC_CURRENT}
	PARENT_SCOPE
)
set(PUBLIC_HEADER ${PUBLIC_HEADER}
	${HEAD_DIR_CURRENT}
	PARENT_SCOPE
)
