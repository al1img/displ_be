project(UnitTests)

################################################################################
# Includes
################################################################################

include_directories(
	.
	../src/displayBackend
	${DRM_INCLUDE_DIRS}
)

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

target_link_libraries(unit_tests xenbemock pthread)

################################################################################
# Libraries
################################################################################

add_test(NAME Test COMMAND unit_tests)
