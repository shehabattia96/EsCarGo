## Please run install.sh before trying to build this to obtain all the dependencies and toolchain.
cmake_minimum_required( VERSION 3.10 FATAL_ERROR )
project( EsCarGo VERSION 0.0.1 DESCRIPTION "An autonomous Go-Kart")

set(CINDER_EXTRN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/Cinder-Simulation-App/externals/Cinder" CACHE STRING "Path to Cinder")
set(PHYSX_EXTRN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/PhysX/physx" CACHE STRING "Path to PhysX")

# set(VCPKG_EXTRN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/vcpkg" CACHE STRING "Path to vcpkg")
# set(VCPKG_TOOLCHAIN "${VCPKG_EXTRN_PATH}/scripts/buildsystems/vcpkg.cmake")
# set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN}")

add_subdirectory(simulation)