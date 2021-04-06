include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/include
			${STDBOOL_H_PATH} ${STDINT_H_PATH})
link_directories(${mosquitto_SOURCE_DIR})

add_library(mosquitto_disconnections SHARED mosquitto_disconnections.c)
set_target_properties(mosquitto_disconnections PROPERTIES
	POSITION_INDEPENDENT_CODE 1
)
set_target_properties(mosquitto_disconnections PROPERTIES PREFIX "")
if(WIN32)
	target_link_libraries(mosquitto_disconnections mosquitto)
endif(WIN32)

# Don't install, these are example plugins only.
install(TARGETS mosquitto_disconnections RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
