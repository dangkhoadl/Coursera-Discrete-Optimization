#!/usr/bin/python
# -*- coding: utf-8 -*-

import subprocess
import os


def run_cpp(cppFilename, inputData):
    # Compile C++
    process = subprocess.Popen([
        "g++",
        "-w", "-O2", "-std=c++11",
        str(cppFilename)])
    process.wait()

    # Writes the inputData to a temporay file
    tmp_file_name = "tmp.data"
    tmp_file = open(tmp_file_name, 'w')
    tmp_file.write(inputData)
    tmp_file.close()

    # Run a.out then get stdout, stderr
    out, err = subprocess.Popen(
        ["./a.out", "tmp.data"],
        shell=False, stdout=subprocess.PIPE).communicate()

    # Removes the temporay files
    os.remove(tmp_file_name)
    os.remove("./a.out")

    # Return result
    return str(out.decode('utf-8'))


def solve_it(inputData):
    return run_cpp("main.cpp", inputData)


if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        file_location = sys.argv[1].strip()
        with open(file_location, 'r') as input_data_file:
            input_data = input_data_file.read()
        print(solve_it(input_data))
    else:
        print('This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/gc_4_1)')
