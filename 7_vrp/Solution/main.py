#!/usr/bin/env python3
import sys
from MIP_Exact import MIP_Exact
from MIP_PetalClustering import MIP_PetalClustering

def read_input(testcase_f_path:str) -> None:
    global N, V, C
    global dc
    global X, Y

    with open(testcase_f_path, 'r+', encoding='utf-8') as file_obj:
        lines = list(file_obj)
        N, V, C = lines[0].strip().split(' ')
        N, V, C = int(N), int(V), int(C)

        dc, X, Y = [0]*N, [0.0]*N, [0.0]*N
        for i, line in enumerate(lines[1:]):
            dc[i], X[i], Y[i] = line.strip().split(' ')

if __name__ == "__main__":
    warning = '''This test requires an input file.
Please select one from the data directory. (i.e. python main.py ./data/fl_100_1)'''
    assert len(sys.argv) == 2, warning

    # Get testcase content
    testcase_f_path = sys.argv[1].strip()
    read_input(testcase_f_path)

    # Solve
    if N <= 100:
        mip = MIP_Exact(testcase_f_path)
        mip.solve(N, V, C,
            dc=dc, X=X, Y=Y)
    else:
        petal = MIP_PetalClustering(testcase_f_path)
        petal.solve(N, V, C,
            dc=dc, X=X, Y=Y)
