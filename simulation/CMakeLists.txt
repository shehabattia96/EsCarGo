message("Building simulation") 
project( Simulation ) 
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/" ABSOLUTE )

include("${CINDER_EXTRN_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

#### Build PhysX library ####
# PHYSX_PATH - path to the `{cloned repository}/physx` directory git://github.com/NVIDIAGameWorks/PhysX.git
set( PHYSX_ROOT_DIR ${PHYSX_EXTRN_PATH} ) #This is needed for ${PHYSX_EXTRN_PATH}/compiler/public/CMakeLists.txt
set( PHYSX_INCLUDE_DIRS ${PHYSX_EXTRN_PATH}/include/ ${PHYSX_EXTRN_PATH}/../pxshared/include/ )
set( PHYSX_LIBRARIES
	PhysXExtensions
	PhysX
	PhysXPvdSDK
	PhysXVehicle
	PhysXCharacterKinematic
	PhysXCooking
	PhysXCommon
	PhysXFoundation
	# SnippetUtils
)

if(NOT EXISTS ${BUILD_OS})
    set(BUILD_OS "windows" CACHE STRING "OS to build PhysX for")
endif()
message("build os: ${BUILD_OS}")
set(TARGET_BUILD_PLATFORM ${BUILD_OS}) # has to match the TARGET_BUILD_PLATFORM in ${PHYSX_EXTRN_PATH}/physix/buildtools/cmake_generate_projects.py
set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
set(PX_GENERATE_STATIC_LIBRARIES OFF CACHE BOOL "Generate static libraries")
set(PX_FLOAT_POINT_PRECISE_MATH OFF CACHE BOOL "Float point precise math")
set(NV_USE_STATIC_WINCRT ON CACHE BOOL "Use the statically linked windows CRT")
set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
set(PXSHARED_PATH ${PHYSX_EXTRN_PATH}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_EXTRN_PATH}/../externals/cmakemodules)
set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
  
# Call into PhysX's CMake scripts
add_subdirectory(${PHYSX_EXTRN_PATH}/compiler/public externals/physx)

SET(CMAKE_BUILD_TYPE "" CACHE PATH "Current build configuration")

#### Build Cinder dependencies and main executable ####
ci_make_app(
	SOURCES		${APP_PATH}/Simulation.cpp
	INCLUDES	${APP_PATH}/include ${PHYSX_INCLUDE_DIRS}
	CINDER_PATH ${CINDER_EXTRN_PATH}
  	RESOURCES   ${APP_PATH}/resources/cinder_app_icon.ico
	LIBRARIES ${PHYSX_LIBRARIES} #same as `target_link_libraries(Simulation cinder ${PHYSX_LIBRARIES})`
)

#### Windows only: Copy the Physx dll files to the simulation executable####

if (TARGET_BUILD_PLATFORM STREQUAL "windows")
	# References NvidiaBuildOptions.cmake to figure out if system is 32/64 bit
	IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
		SET(LIBPATH_SUFFIX "64")
	ELSE()
		SET(LIBPATH_SUFFIX "32")
	ENDIF()
	GetPlatformBinName(PLATFORM_BIN_NAME ${LIBPATH_SUFFIX})
	set(PhysxOutputPath ${PX_OUTPUT_LIB_DIR}/bin/${PLATFORM_BIN_NAME}/)
	message("Physx Output Path: " ${PhysxOutputPath})

	# Create a command to run every time target is built:
	add_custom_command(TARGET Simulation POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${PhysxOutputPath}" "$<TARGET_FILE_DIR:Simulation>/..")
endif()


message("Done building simulation")  
