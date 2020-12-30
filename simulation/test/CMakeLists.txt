message("Building Simulation_Tests")
cmake_minimum_required(VERSION 3.11)
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Simulation_Tests C CXX)

get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/" ABSOLUTE )

#### VCPKG
include("${APP_PATH}/../FindPhysX.cmake")

#### Catch2
find_package(Catch2 CONFIG REQUIRED) # uses vcpkg toolchain
set(VCPKG_LIBRARIES ${VCPKG_LIBRARIES} Catch2::Catch2)

#### Test sources
set(
    sources
    ${APP_PATH}/test_main.cpp
    ${APP_PATH}/test_physx_integration.cpp
)

#### Build Cinder dependencies and main executable ####
include("${CINDER_EXTRN_PATH}/proj/cmake/modules/cinderMakeApp.cmake")
ci_make_app(
	SOURCES		${sources} ${snippetvehiclecommon_SOURCES}
	INCLUDES	${PHYSX_INCLUDE_DIRS} ${VCPKG_INCLUDES}
	CINDER_PATH ${CINDER_EXTRN_PATH}
	LIBRARIES   ${PHYSX_LIBRARIES} ${VCPKG_LIBRARIES} #same as `target_link_libraries(Simulation cinder ${PHYSX_LIBRARIES})`
)

#### copy resources directory to build dir:
add_custom_command(TARGET Simulation_Tests POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${APP_PATH}/resources/" "$<TARGET_FILE_DIR:Simulation_Tests>/resources")


message("Done building Simulation_Tests")   