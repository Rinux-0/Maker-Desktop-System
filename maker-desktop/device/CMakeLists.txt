if (DEFINED CONFIG_DEVICE_KEYBOARD)
	add_subdirectory_if_exist(keyboard)
endif()
if (DEFINED CONFIG_DEVICE_KEYPAD)
	add_subdirectory_if_exist(keypad)
endif()
if (DEFINED CONFIG_DEVICE_MOUSE)
	add_subdirectory_if_exist(mouse)
endif()
if (DEFINED CONFIG_DEVICE_POWER)
	add_subdirectory_if_exist(power)
endif()
if (DEFINED CONFIG_DEVICE_SOUND)
	add_subdirectory_if_exist(sound)
endif()
if (DEFINED CONFIG_DEVICE_LAMP)
	add_subdirectory_if_exist(lamp)
endif()
if (DEFINED CONFIG_DEVICE_RECEIVER)
	add_subdirectory_if_exist(receiver)
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
