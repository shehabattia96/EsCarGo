# Simulation

## Getting Started

Run the install.sh script in the root directory. This will build Cinder, PhysX and the simulation app. Tested on windows but should work on any platform by using the `-o` parameter to specify the platform (has to match the TARGET_BUILD_PLATFORM in ${PHYSX_EXTRN_PATH}/physix/buildtools/cmake_generate_projects.py).

## Goals of the simulator

1. Recreate a parametric model of a life-sized go-kart in the Physx engine. This will allow us to test different motor powers, wheel radii, sensors etc..
2. Render and drive the Go-Kart around the simulation world using a joy-con. Collect sensor data in an mqtt pipeline.
3. Generate courses for the Go-Kart to race around in.
4. Introduce more Go-Karts and have the Go-Karts race autonomously.

