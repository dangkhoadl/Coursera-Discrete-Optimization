#!/usr/bin/python3
# -*- coding: utf-8 -*-
'''
Run:
    python solver.py ./data/<test-case>
'''
import subprocess
import sys, os


def run_cpp(test_case: str, cpp_main_fpath: str='Solution/main.cpp') -> str:
    file_name = cpp_main_fpath.split('/')[-1].replace('.cpp', '')

    # Compile C++
    if not os.path.exists(f'{file_name}.exe'):
        process = subprocess.Popen([
            'g++', '-w', '-O2', '-std=c++11', '-o', f'{file_name}.exe', str(cpp_main_fpath)])
        process.wait()

    # Run main.exe then get stdout, stderr
    process = subprocess.Popen(
        [f'./{file_name}.exe'],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate(input=test_case.encode('utf-8'))

    # Removes the temporary files
    # os.remove(f'{file_name}.exe')

    # Return result
    if stderr:
        sys.stderr.write(str(stderr.decode('utf-8')) + '\n')
        exit(1)
    return str(stdout.decode('utf-8'))

def run_py(test_case: str, py_main_fpath: str='Solution/main.py') -> str:
    # Run main.py then get stdout, stderr
    process = subprocess.Popen(
        ['python', py_main_fpath],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate(input=test_case.encode('utf-8'))

    # Return result
    if stderr:
        sys.stderr.write(str(stderr.decode('utf-8')) + '\n')
        exit(1)
    return str(stdout.decode('utf-8'))


def solve_it(test_case: str):
    '''submit.py will parse this function'''
    return run_py(test_case)


if __name__ == '__main__':
    warning = '''This test requires an input file.
Please select one from the data directory. (i.e. python solver.py ./data/ks_4_0)'''
    assert len(sys.argv) == 2, warning

    # Get testcase content
    testcase_f_path = sys.argv[1].strip()
    with open(testcase_f_path, 'r+', encoding='utf-8') as file_obj:
        test_case = file_obj.read()

    # Solve
    print(solve_it(test_case), end='')
