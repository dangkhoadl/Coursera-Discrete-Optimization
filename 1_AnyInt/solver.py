#!/usr/bin/python3
# -*- coding: utf-8 -*-

import subprocess

def run_cpp(cppFilename):
    # Compile C++
    process = subprocess.Popen([
        "g++",
        "-w", "-O2", "-std=c++11",
        str(cppFilename)])
    process.wait()

    # Run a.out then get stdout, stderr
    out, err = subprocess.Popen(
        "./a.out",
        shell=True, stdout=subprocess.PIPE).communicate()

    # Clear a.out
    subprocess.Popen(["rm", "-rf", "a.out"])

    # Return result
    return str(out.decode('utf-8'))


def solve_it(input_data):
    '''return a positive integer, as a string'''
    return run_cpp("main.cpp")


if __name__ == '__main__':
    print('This script submits the integer: %s\n' % solve_it(''))
