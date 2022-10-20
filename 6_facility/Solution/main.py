#!/usr/bin/env python3
import sys
from MIP_Exact import MIP_Exact
from MIP_Clustering import MIP_Clustering


def read_input(testcase_f_path:str) -> None:
    global N, M
    global sf, capf, xf, yf
    global dc, xc, yc

    with open(testcase_f_path, 'r+', encoding='utf-8') as file_obj:
        lines = list(file_obj)
        N, M = lines[0].strip().split(' ')
        N, M = int(N), int(M)

        sf, capf, xf, yf = [0.0]*N, [0]*N, [0.0]*N, [0.0]*N
        for i, line in enumerate(lines[1:N+1]):
            sf[i], capf[i], xf[i], yf[i] = line.strip().split(' ')

        dc, xc, yc = [0]*M, [0.0]*M, [0.0]*M
        for i, line in enumerate(lines[N+1:]):
            dc[i], xc[i], yc[i] = line.strip().split(' ')


if __name__ == "__main__":
    warning = '''This test requires an input file.
Please select one from the data directory. (i.e. python main.py ./data/fl_100_1)'''
    assert len(sys.argv) == 2, warning

    # Get testcase content
    testcase_f_path = sys.argv[1].strip()
    read_input(testcase_f_path)

    if N*M <= 10000:
        mip = MIP_Exact(testcase_f_path)
        mip.solve(N, M,
            sf=sf, capf=capf, xf=xf, yf=yf,
            dc=dc, xc=xc, yc=yc)
    else:
        kmean = MIP_Clustering(testcase_f_path)
        kmean.solve(N, M,
            sf=sf, capf=capf, xf=xf, yf=yf,
            dc=dc, xc=xc, yc=yc)
