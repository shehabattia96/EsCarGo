
#### Add PhysX library from vcpkg toolchain
set(PHYSX_LIBPATH_SUFFIX "_64.lib")
find_path(PHYSX_INCLUDE_DIR PxConfig.h)  # uses vcpkg toolchain
find_library(PHYSX_LIBRARY PhysX)  # uses vcpkg toolchain
get_filename_component(PHYSX_LIBRARY_DIR "${PHYSX_LIBRARY}" PATH)

if(${BUILD_OS} STREQUAL "windows")
	set(PHYSX_INCLUDE_DIR "${VCPKG_EXTRN_PATH}/installed/x64-windows-static/include")  # uses vcpkg toolchain
	set(PHYSX_LIBRARY_DIR "${VCPKG_EXTRN_PATH}/installed/x64-windows-static/debug/lib")  # uses vcpkg toolchain
	set(PHYSX_LIBPATH_SUFFIX "_static_64.lib") # static library needed
endif()

set( PHYSX_LIBRARIES_VCPKG
	${PHYSX_LIBRARY_DIR}/PhysXExtensions${PHYSX_LIBPATH_SUFFIX}
	${PHYSX_LIBRARY_DIR}/PhysX${PHYSX_LIBPATH_SUFFIX}
	${PHYSX_LIBRARY_DIR}/PhysXPvdSDK${PHYSX_LIBPATH_SUFFIX}
	${PHYSX_LIBRARY_DIR}/PhysXVehicle${PHYSX_LIBPATH_SUFFIX}
	${PHYSX_LIBRARY_DIR}/PhysXCharacterKinematic${PHYSX_LIBPATH_SUFFIX}
	${PHYSX_LIBRARY_DIR}/PhysXCooking${PHYSX_LIBPATH_SUFFIX}
	${PHYSX_LIBRARY_DIR}/PhysXCommon${PHYSX_LIBPATH_SUFFIX}
	${PHYSX_LIBRARY_DIR}/PhysXFoundation${PHYSX_LIBPATH_SUFFIX}
	# ${PHYSX_LIBRARY_DIR}/SnippetUtils${PHYSX_LIBPATH_SUFFIX}
)

#### VCPKG
set(VCPKG_INCLUDES ${PHYSX_INCLUDE_DIR})
set(VCPKG_LIBRARIES ${PHYSX_LIBRARIES_VCPKG})

#### Add snippetvehiclecommon sources from PhysX git repo submodule ####
set (snippetvehiclecommon_DIR "${PHYSX_EXTRN_PATH}/snippets/snippetvehiclecommon")
include_directories("${snippetvehiclecommon_DIR}")
set (snippetvehiclecommon_SOURCES 
	${snippetvehiclecommon_DIR}/SnippetVehicleSceneQuery.cpp 
	${snippetvehiclecommon_DIR}/SnippetVehicleFilterShader.cpp
	${snippetvehiclecommon_DIR}/SnippetVehicleTireFriction.cpp
	${snippetvehiclecommon_DIR}/SnippetVehicleCreate.cpp
	${snippetvehiclecommon_DIR}/SnippetVehicle4WCreate.cpp
)