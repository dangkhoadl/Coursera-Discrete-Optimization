#!/usr/bin/env bash
#
set -u           # Detect unset variable
set -o pipefail  # Exit if any of the commands in the pipeline will
                 # return non-zero return code


# Compile + benchmark all testcases in data/
g++ -w -O2 -std=c++11 -o main.exe Solution/main.cpp
./main.exe  < data/tsp_51_1
