SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )" # copypasta from https://stackoverflow.com/a/4774063/
sh $SCRIPT_DIR/install.sh -b
gdb $SCRIPT_DIR/build/simulation/debug/simulation