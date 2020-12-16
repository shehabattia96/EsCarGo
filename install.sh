#!/bin/bash

## Script clones submodules and attempts to build them using CMAKE.
## Run with -o to specify build OS (currently for PhysX only). -v Release|Debug to build variants.

SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )" # copypasta from https://stackoverflow.com/a/4774063/

# Options
BUILD_VARIANT=Debug #or Release
BUILD_OS=windows

# Parse args - references https://stackoverflow.com/a/33826763/9824103
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -v|--variant) BUILD_VARIANT="$2"; shift ;;
        -o|--os) BUILD_OS="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done
echo "BUILD_VARIANT is $BUILD_VARIANT. Set with the -v flag (Debug|Release)."


# Clone submodules
git submodule update --init --recursive


# Build Cinder -- assumes it's under externals/Cinder and assumes you have CMAKE installed and configured.
echo "Building Cinder in externals/Cinder-Simulation-App"
CINDER_SIMULATION_APP_DIR="$SCRIPT_DIR/externals/Cinder-Simulation-App/"
sh $CINDER_SIMULATION_APP_DIR/install.sh

# Build CinderSimulationApp
echo "Building PhysX and Simulation"
cmake -DBUILD_OS=$BUILD_OS "$SCRIPT_DIR" -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -H$SCRIPT_DIR -B$SCRIPT_DIR/build
cmake --build ./build --config $BUILD_VARIANT --target ALL_BUILD -- /maxcpucount:18

echo "Done."

