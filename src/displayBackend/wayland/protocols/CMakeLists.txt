################################################################################
# Find protocols
################################################################################

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
	execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=pkgdatadir wayland-protocols
					OUTPUT_VARIABLE WL_PROTOCOLS_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if(!WL_PROTOCOLS_PATH)
	message(WARNING "No wayland protocols found. XDG will be disabled")
else()
	message(STATUS "Wayland protocol path: ${WL_PROTOCOLS_PATH}")
endif()

find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)

################################################################################
# DRM
################################################################################

if(WITH_DRM)
	add_custom_command(
		OUTPUT  wayland-drm-client-protocol.h
		COMMAND ${WAYLAND_SCANNER_EXECUTABLE} client-header
				< ${CMAKE_CURRENT_LIST_DIR}/wayland-drm.xml
				> ${CMAKE_CURRENT_BINARY_DIR}/wayland-drm-client-protocol.h
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/wayland-drm.xml
	)

	add_custom_command(
		OUTPUT  wayland-drm-protocol.c
		COMMAND ${WAYLAND_SCANNER_EXECUTABLE} code
				< ${CMAKE_CURRENT_LIST_DIR}/wayland-drm.xml
				> ${CMAKE_CURRENT_BINARY_DIR}/wayland-drm-protocol.c
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/wayland-drm.xml
	)

	add_library(wayland_drm_protocol STATIC
		${CMAKE_CURRENT_BINARY_DIR}/wayland-drm-client-protocol.h
		${CMAKE_CURRENT_BINARY_DIR}/wayland-drm-protocol.c
	)
endif()

################################################################################
# Xdg Shell
################################################################################

set(XDG_PROTOCOL_NAME "xdg-shell-unstable-v6")

if(EXISTS ${WL_PROTOCOLS_PATH}/unstable/xdg-shell/${XDG_PROTOCOL_NAME}.xml)
	
	set(WITH_XDG_SHELL ON PARENT_SCOPE)

	add_custom_command(
		OUTPUT  ${XDG_PROTOCOL_NAME}.h
		COMMAND ${WAYLAND_SCANNER_EXECUTABLE} client-header
				< ${WL_PROTOCOLS_PATH}/unstable/xdg-shell/${XDG_PROTOCOL_NAME}.xml
				> ${CMAKE_CURRENT_BINARY_DIR}/${XDG_PROTOCOL_NAME}.h
		DEPENDS ${WL_PROTOCOLS_PATH}/unstable/xdg-shell/${XDG_PROTOCOL_NAME}.xml
	)
	
	
	add_custom_command(
		OUTPUT  ${XDG_PROTOCOL_NAME}.c
		COMMAND ${WAYLAND_SCANNER_EXECUTABLE} code
				< ${WL_PROTOCOLS_PATH}/unstable/xdg-shell/${XDG_PROTOCOL_NAME}.xml
				> ${CMAKE_CURRENT_BINARY_DIR}/${XDG_PROTOCOL_NAME}.c
		DEPENDS ${WL_PROTOCOLS_PATH}/unstable/xdg-shell/${XDG_PROTOCOL_NAME}.xml
	)
	
	add_library(${XDG_PROTOCOL_NAME} STATIC
		${CMAKE_CURRENT_BINARY_DIR}/${XDG_PROTOCOL_NAME}.h
		${CMAKE_CURRENT_BINARY_DIR}/${XDG_PROTOCOL_NAME}.c
	)
endif()

