#!/usr/bin/env bash
#
set -u           # Detect unset variable
set -o pipefail  # Exit if any of the commands in the pipeline will
                 # return non-zero return code

# Config
export OUTPUT_DIR="assets"
OUTPUT_TXT="${OUTPUT_DIR}/results_submission.txt"
MAX_PROCESS=1

####### Functions #######
function solve_a_testcase {
    testcase_fpath=$1

    # Run for 30 min
    timeout 30m ./main.exe ${testcase_fpath}
    #
}
export -f solve_a_testcase;


####### Main #######
mkdir -p "${OUTPUT_DIR}" submission/
g++ -w -O2 -std=c++11 "Solution/main.cpp" -o "main.exe"

cat _coursera |  grep './data' | cut -d',' -f2 \
    | sort -t'_' -k2 -n \
    | xargs --max-procs=${MAX_PROCESS} -I'%' bash -c "solve_a_testcase %"

rm main.exe

# cat $(find submission/ -type f -name "out_*" \
#     | sort -t'_' -k3 -n) \
#     > ${OUTPUT_TXT}

# Clean
# rm -r submission/ 2> /dev/null || true
