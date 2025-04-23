if(DEFINED CONFIG_HEALTH_FINGERPRINT)
	add_subdirectory_if_exist(fingerprint)
else()
	if(DEFINED CONFIG_HEALTH_DISTANCE)
		add_subdirectory_if_exist(distance)
	endif()
	if(DEFINED CONFIG_HEALTH_HEART)
		add_subdirectory_if_exist(heart)
	endif()
	if(DEFINED CONFIG_HEALTH_TEMPERATURE)
		add_subdirectory_if_exist(tempreture)
	endif()
endif()



set(SRC_CURRENT
	${CMAKE_CURRENT_SOURCE_DIR}/health.c
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
