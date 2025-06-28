if(DEFINED CONFIG_DEVICE_DYNAMIC)
	add_subdirectory_if_exist(dynamic)
elseif(DEFINED CONFIG_DEVICE_HEALTH)
	add_subdirectory_if_exist(health)
elseif(DEFINED CONFIG_DEVICE_KEYBOARD)
	add_subdirectory_if_exist(keyboard)
elseif(DEFINED CONFIG_DEVICE_KEYPAD)
	add_subdirectory_if_exist(keypad)
elseif(DEFINED CONFIG_DEVICE_MOUSE)
	add_subdirectory_if_exist(mouse)
elseif(DEFINED CONFIG_DEVICE_RECEIVER)
	add_subdirectory_if_exist(receiver)
elseif(DEFINED CONFIG_DEVICE_TRINITY)
	add_subdirectory_if_exist(trinity)
else()
	message(FATAL_ERROR "This device can't be recognized!")
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
