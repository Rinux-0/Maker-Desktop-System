add_subdirectory_if_exist(keypad)
add_subdirectory_if_exist(sle_uart_1v8)


set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/demo.c)

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
