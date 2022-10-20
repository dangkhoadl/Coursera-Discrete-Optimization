#!/usr/bin/env bash
#
set -u           # Detect unset variable
set -o pipefail  # Exit if any of the commands in the pipeline will
                 # return non-zero return code

# Config
export OUTPUT_DIR="assets"
OUTPUT_TXT="${OUTPUT_DIR}/results_submission.txt"
MAX_PROCESS=6

####### Functions #######
function displaytime {
    local T=$1

    local H=$((T/1000/60/60))
    local M=$((T/1000/60%60))
    local S=$((T/1000%60))
    local ms=$((T%1000))

    (( $H > 0 )) && printf '%d hr ' $H
    (( $M > 0 )) && printf '%d min ' $M
    (( $S > 0 )) && printf '%d sec ' $S
    (( $ms > 0 )) && printf '%d ms\n' $ms
}
export -f displaytime;

function solve_a_testcase {
    testcase_fpath=$1
    testcase_str=$(echo ${testcase_fpath} | rev | cut -d'/' -f1 | rev)

    start_time=$(date +%s%3N)
    # 15 min timeout
    stdout=$(timeout 15m python solver.py ${testcase_fpath})
    exit_code=$?
    #
    end_time=$(date +%s%3N)
    runtime=$(displaytime $((end_time-start_time)))

    if [[ ${exit_code} == 0 ]]; then
        cat > submission/out_${testcase_str} <<EOF
${testcase_fpath}: ${runtime}
${stdout}

EOF
    elif [[ ${exit_code} == 124 ]]; then
        cat > submission/out_${testcase_str} <<EOF
${testcase_fpath}: <TLE - ${runtime}>

EOF
    else
        cat > submission/out_${testcase_str} <<EOF
${testcase_fpath}: <ERR ${exit_code}>

EOF
        exit 1
    fi
}
export -f solve_a_testcase;


####### Main #######
mkdir -p "${OUTPUT_DIR}" submission/

cat _coursera |  grep './data' | cut -d',' -f2 \
    | sort -t'_' -k2 -n \
    | xargs --max-procs=${MAX_PROCESS} -I'%' bash -c "solve_a_testcase %"

cat $(find submission/ -type f -name "out_*" \
    | sort -t'_' -k3 -n) \
    > ${OUTPUT_TXT}

# Clean
# rm -r submission/ 2> /dev/null || true
