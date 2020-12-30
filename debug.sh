SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )" # copypasta from https://stackoverflow.com/a/4774063/
vcpkg_DIR="$SCRIPT_DIR/externals/vcpkg"
BUILD_OS=windows # default, can be linux or mac too. use -o flag.

function log { echo "$1" | tee -a $LOG_FILENAME; }

# Parse args - references https://stackoverflow.com/a/33826763/9824103
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -o|--os) BUILD_OS="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

log "Building Simulation"
cmake -DBUILD_OS=$BUILD_OS "$SCRIPT_DIR" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$vcpkg_DIR/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -H$SCRIPT_DIR -B$SCRIPT_DIR/build
cmake --build ./build --config Debug --target ALL_BUILD -- /maxcpucount:18

# gdb $SCRIPT_DIR/build/simulation/debug/Simulation