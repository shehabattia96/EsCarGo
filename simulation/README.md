# Simulation

## Getting Started

Run the install.sh script in the root directory. This will build Cinder, PhysX and the simulation app. Tested on windows but should work on any platform by using the `-o` parameter to specify the platform (has to match the TARGET_BUILD_PLATFORM in ${PHYSX_EXTRN_PATH}/physix/buildtools/cmake_generate_projects.py).

## Goals of the simulator

1. Recreate a parametric model of a life-sized go-kart in the Physx engine. This will allow us to test different motor powers, wheel radii, sensors etc..
2. Render and drive the Go-Kart around the simulation world using a joy-con. Collect sensor data in an mqtt pipeline.
3. Generate courses for the Go-Kart to race around in.
4. Introduce more Go-Karts and have the Go-Karts race autonomously.

## High Level Process

1. GUI window, including sidebar is created in Simulation.h/cpp. During setup(), the world is initialized with all the meshes that will take part in the simulation. The meshes are created as Cinder Source shapes, then converted into GL batches.
2. PhysX uses its own library's shapes, so the meshes created in Cinder Sources are re-created in PhysX, along with collision boundaries and event queries.
3. On every draw cycle in Cinder, the PhysX simulate function is called, blocking drawing until new poses for each active actor are obtained.
4. The GL matricies are transformed in Cinder by the new world pose provided by PhysX.

## Testing

Testing executable builds under `/build/Simulation_Tests/debug/Simulation_Tests`