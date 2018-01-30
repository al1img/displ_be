################################################################################
# Includes
################################################################################

################################################################################
# Sources
################################################################################

if(WITH_DRM)
	add_subdirectory(drm)
endif()

if(WITH_WAYLAND)
	add_subdirectory(wayland)
endif()

set(SOURCES
	BuffersStorage.cpp
	DisplayBackend.cpp
	DisplayCommandHandler.cpp
)

################################################################################
# Targets
################################################################################

add_library(display STATIC ${SOURCES})

################################################################################
# Libraries
################################################################################

if(WITH_WAYLAND)

	target_link_libraries(display display_wayland ${WAYLAND_LIBRARIES})

if(WITH_IVI_EXTENSION)
	target_link_libraries(display ${IVI_APP_LIBRARIES} )
endif()

endif()

if(WITH_DRM)
	target_link_libraries(display display_drm ${DRM_LIBRARIES})
endif()

target_include_directories(display PUBLIC . )
