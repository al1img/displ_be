project(UnitTests)

################################################################################
# Includes
################################################################################

include_directories(
	.
)

if(WITH_DRM OR WITH_WAYLAND)
	include_directories(../src/displayBackend)
endif()

if(WITH_DRM)
	include_directories(${DRM_INCLUDE_DIRS})
endif()

################################################################################
# Sources
################################################################################

add_subdirectory(mocks)

set(TEST_SOURCES
	UnitTests.cpp
)

if(WITH_DRM)
	add_subdirectory(drm)
	list(APPEND TEST_SOURCES $<TARGET_OBJECTS:test_drm>)
endif()

if(WITH_WAYLAND)
	add_subdirectory(wayland)
	list(APPEND TEST_SOURCES $<TARGET_OBJECTS:test_wayland>)
endif()

################################################################################
# Targets
################################################################################

add_executable(unit_tests ${TEST_SOURCES})

if(WITH_DRM)
	target_link_libraries(unit_tests display_drm drm_mock)
	set_target_properties(unit_tests PROPERTIES LINK_FLAGS
		"-Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=mmap -Wl,--wrap=munmap"
	)
endif()

if(WITH_WAYLAND)
	target_link_libraries(unit_tests display_wayland wayland_mock)
endif()

target_link_libraries(unit_tests xenbemock pthread)

################################################################################
# Libraries
################################################################################

add_test(NAME Test COMMAND unit_tests)
