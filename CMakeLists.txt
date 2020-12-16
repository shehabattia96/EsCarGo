cmake_minimum_required( VERSION 3.10 FATAL_ERROR )
project( EsCarGo VERSION 0.0.1 DESCRIPTION "A Go-Kart")

set(CINDER_EXTRN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/Cinder-Simulation-App/externals/Cinder" CACHE STRING "Path to Cinder")
set(PHYSX_EXTRN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/PhysX/physx" CACHE STRING "Path to PhysX")

add_subdirectory(simulation)