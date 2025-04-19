if (DEFINED CONFIG_DEVICE_RECEIVER)
	add_subdirectory_if_exist(receiver)
elseif(DEFINED CONFIG_DEVICE_KEYBOARD)
	add_subdirectory_if_exist(keyboard)
elseif(DEFINED CONFIG_DEVICE_KEYPAD)
	add_subdirectory_if_exist(keypad)
elseif(DEFINED CONFIG_DEVICE_LAMP)
	add_subdirectory_if_exist(lamp)
elseif(DEFINED CONFIG_DEVICE_MOUSE)
	add_subdirectory_if_exist(mouse)
elseif(DEFINED CONFIG_DEVICE_UNION)
	add_subdirectory_if_exist(union)
endif()



set(SRC_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}/device.c
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
