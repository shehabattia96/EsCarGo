message("Building simulation") 
cmake_minimum_required(VERSION 3.11)
set( CMAKE_VERBOSE_MAKEFILE ON )
project( Simulation C CXX )

get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/" ABSOLUTE )

#### VCPKG
set (snippetvehiclecommon_DIR "${APP_PATH}/include/snippetvehiclecommon")
include("${APP_PATH}/FindPhysX.cmake")

#### Build Cinder dependencies and main executable ####
include("${CINDER_EXTRN_PATH}/proj/cmake/modules/cinderMakeApp.cmake")
ci_make_app(
	SOURCES		${APP_PATH}/Simulation.cpp ${snippetvehiclecommon_SOURCES}
	INCLUDES	${APP_PATH}/include ${PHYSX_INCLUDE_DIRS} ${VCPKG_INCLUDES}
	CINDER_PATH ${CINDER_EXTRN_PATH}
  	RESOURCES   ${APP_PATH}/resources/cinder_app_icon.ico
	LIBRARIES   ${PHYSX_LIBRARIES} ${VCPKG_LIBRARIES} #same as `target_link_libraries(Simulation cinder ${PHYSX_LIBRARIES})`
)

#### copy resources directory to build dir:
add_custom_command(TARGET Simulation POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${APP_PATH}/resources/" "$<TARGET_FILE_DIR:Simulation>/resources")

#### tests	
include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()


message("Done building simulation")  
