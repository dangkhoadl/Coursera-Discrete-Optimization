#!/usr/bin/python3
# -*- coding: utf-8 -*-
import subprocess
import sys, os


def run_cpp(cpp_main_fpath: str='Solution/main.cpp') -> str:
    # Compile C++
    process = subprocess.Popen([
        'g++', '-w', '-O2', '-std=c++11', '-o', 'main.exe', str(cpp_main_fpath)])
    process.wait()

    # Run main.exe then get stdout, stderr
    process = subprocess.Popen(
        ['./main.exe'],
        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()

    # Removes the temporay files
    os.remove('main.exe')

    # Return result
    if stderr:
        with open('stderr.log', 'w+', encoding='utf-8') as file_obj:
            file_obj.write(str(stderr.decode('utf-8')))
    return str(stdout.decode('utf-8'))


def solve_it(input_data):
    return run_cpp()


if __name__ == '__main__':
    assert len(sys.argv) == 1, 'No input required'
    print(solve_it(''), end='')
