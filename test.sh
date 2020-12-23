SCRIPT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )" # copypasta from https://stackoverflow.com/a/4774063/

LOG_FILENAME="$SCRIPT_DIR/.test_script.log"

function log { echo "$1" | tee -a $LOG_FILENAME; }
log ""
log "----------$(date)"


"$SCRIPT_DIR/build/Simulation_Tests\debug\Simulation_Tests" | tee -a $LOG_FILENAME
log "Done. $(date)"