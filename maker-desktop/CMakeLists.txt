if(DEFINED CONFIG_DEVICE)
	if(DEFINED CONFIG_DEVICE_RECEIVER)
		# add_definitions(-DDEVICE_NAME=receiver)
	elseif(DEFINED CONFIG_DEVICE_KEYBOARD)
		# add_definitions(-DDEVICE_NAME=keyboard)
	elseif(DEFINED CONFIG_DEVICE_KEYPAD)
		# add_definitions(-DDEVICE_NAME=keypad)
	elseif(DEFINED CONFIG_DEVICE_KNOB)
		# add_definitions(-DDEVICE_NAME=knob)
	elseif(DEFINED CONFIG_DEVICE_LAMP)
		# add_definitions(-DDEVICE_NAME=lamp)
	elseif(DEFINED CONFIG_DEVICE_MOUSE)
		# add_definitions(-DDEVICE_NAME=mouse)
	elseif(DEFINED CONFIG_DEVICE_NFC)
		# add_definitions(-DDEVICE_NAME=nfc)
	elseif(DEFINED CONFIG_DEVICE_UNION)
		# add_definitions(-DDEVICE_NAME=union)
	else()
		message(FATAL_ERROR "This device can't be recognized!")
	endif()
elseif(DEFINED CONFIG_HEALTH)
	if(DEFINED CONFIG_HEALTH_DISTANCE)
		# add_definitions(-DHEALTH_NAME=distance)
	elseif(DEFINED CONFIG_HEALTH_FINGERPRINT)
		# add_definitions(-DHEALTH_NAME=fingerprint)
	elseif(DEFINED CONFIG_HEALTH_HEART)
		# add_definitions(-DHEALTH_NAME=heart)
	elseif(DEFINED CONFIG_HEALTH_TEMPERATURE)
		# add_definitions(-DHEALTH_NAME=temperature)
	else()
		message(FATAL_ERROR "This health can't be recognized!")
	endif()
else()
	message(FATAL_ERROR "There is no device or health!")
endif()



add_subdirectory_if_exist(comm)
if(DEFINED CONFIG_DEVICE)
	add_subdirectory_if_exist(device)
elseif(DEFINED CONFIG_HEALTH)
	add_subdirectory_if_exist(health)
endif()



file(GLOB_RECURSE SRC_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}/demo.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/*.c
)
set(HEAD_DIR_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/util/def
	${CMAKE_CURRENT_SOURCE_DIR}/util/tool
)



set(SOURCES ${SOURCES}
	${SRC_CURRENT}
	PARENT_SCOPE
)
set(PUBLIC_HEADER ${PUBLIC_HEADER}
	${HEAD_DIR_CURRENT}
	PARENT_SCOPE
)
