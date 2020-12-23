message("Building Simulation_Tests")
cmake_minimum_required( VERSION 3.10 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Simulation_Tests )

include("${CINDER_EXTRN_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

include_directories("${snippetvehiclecommon_DIR}")

get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/" ABSOLUTE )

find_package(Catch2 CONFIG REQUIRED) # uses vcpkg toolchain

set(VCPKG_LIBRARIES ${VCPKG_LIBRARIES} Catch2::Catch2)

set(
    sources
    ${APP_PATH}/test_main.cpp
    ${APP_PATH}/test_physx_integration.cpp
)

ci_make_app(
	SOURCES		${sources} ${snippetvehiclecommon_SOURCES}
	INCLUDES	${PHYSX_INCLUDE_DIRS}
	CINDER_PATH ${CINDER_EXTRN_PATH}
	LIBRARIES   ${PHYSX_LIBRARIES} ${VCPKG_LIBRARIES} #same as `target_link_libraries(Simulation cinder ${PHYSX_LIBRARIES})`
)

# copy resources directory to build dir:
add_custom_command(TARGET Simulation_Tests POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${APP_PATH}/resources/" "$<TARGET_FILE_DIR:Simulation_Tests>/resources")

#### Windows only: Copy the Physx dll files to the SimulationTests executable####
if (TARGET_BUILD_PLATFORM STREQUAL "windows")
    add_custom_command(TARGET Simulation_Tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PhysxOutputPath}" "$<TARGET_FILE_DIR:Simulation_Tests>/..")
endif()


message("Done building Simulation_Tests")   