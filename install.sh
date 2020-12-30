#!/bin/bash

## Script clones submodules and attempts to build them using CMAKE.
    # Recursively clones submodules
    # Installs VCPKG
    # Installs jsoncpp
    # Runs install.sh for externals/Cinder-Simulation-App to build Cinder.
## Run with 
    # -o to specify build OS: windows, linux, mac (currently for PhysX only). Assumes x64.
    # -v Release|Debug to build variants.
    # -b to build only.

SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )" # copypasta from https://stackoverflow.com/a/4774063/
vcpkg_DIR="$SCRIPT_DIR/externals/vcpkg"
CINDER_SIMULATION_APP_DIR="$SCRIPT_DIR/externals/Cinder-Simulation-App/"

# Options
BUILD_VARIANT=Debug #or Release
BUILD_OS=windows # default, can be linux or mac too. use -o flag.
BUILD_ONLY=0

# Parse args - references https://stackoverflow.com/a/33826763/9824103
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -v|--variant) BUILD_VARIANT="$2"; shift ;;
        -o|--os) BUILD_OS="$2"; shift ;;
        -b|--build) BUILD_ONLY=1 ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

LOG_FILENAME="$SCRIPT_DIR/.install_script.log"

function log { echo "$1" | tee -a $LOG_FILENAME; }
log ""
log "----------$(date)"
log "BUILD_VARIANT is $BUILD_VARIANT. Set with the -v flag (Debug|Release)."

if (($BUILD_ONLY == 0)); then

    # Clone submodules
    git submodule update --init --recursive

    # Install vcpkg
    if [ ! -f "$vcpkg_DIR/vcpkg" ]; then
        log "Running $vcpkg_DIR/bootstrap-vcpkg.sh"
        sh "$vcpkg_DIR/bootstrap-vcpkg.sh"
    fi
    log "Running $vcpkg_DIR/vcpkg integrate install"
    "$vcpkg_DIR/vcpkg" integrate install

    # Install catch2
    log "Running $vcpkg_DIR/vcpkg install catch2:x64-$BUILD_OS"
    "$vcpkg_DIR/vcpkg" install catch2:x64-$BUILD_OS

    # Install physx
    physxStaticSuffix=""
    if (($BUILD_OS == "windows"))
        physxStaticSuffix="-static"
    fi
    log "Running $vcpkg_DIR/vcpkg install physx:x64-$BUILD_OS$physxStaticSuffix"
    "$vcpkg_DIR/vcpkg" install physx:x64-$BUILD_OS$physxStaticSuffix

    # Build Cinder -- assumes it's under externals/Cinder-Simulation-App and assumes you have CMAKE installed and configured.
    log "Building Cinder via $CINDER_SIMULATION_APP_DIR/install.sh"
    sh $CINDER_SIMULATION_APP_DIR/install.sh

fi

# Build CinderSimulationApp
log "Building PhysX and Simulation"
cmake -DBUILD_OS=$BUILD_OS "$SCRIPT_DIR" -DCMAKE_TOOLCHAIN_FILE=$vcpkg_DIR/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -H$SCRIPT_DIR -B$SCRIPT_DIR/build
cmake --build ./build --config $BUILD_VARIANT --target ALL_BUILD -- /maxcpucount:18

log "Done. $(date)"

