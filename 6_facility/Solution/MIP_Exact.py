from __future__ import annotations
from typing import List, Tuple
import numpy as np
from pulp import LpMinimize, LpProblem, LpVariable, COIN_CMD
from dataclasses import dataclass
import os


class MIP_Exact(object):
    @dataclass
    class Facility(object):
        def __init__(self, sf:float, capf: float, xf:float, yf:float) -> None:
            self.setup_cost = sf
            self.capacity = capf
            self.x = xf
            self.y = yf

    @dataclass
    class Customer(object):
        def __init__(self, dc:float, xc:float, yc:float) -> None:
            self.demand = dc
            self.x = xc
            self.y = yc

    def __init__(self, testcase_f_path) -> None:
        self._N, self._M = None, None
        self._facs = None
        self._cuss = None
        self._test_case = testcase_f_path.split('/')[-1]

    def __write_best(self, objective:float, assignment:List[int]):
        if not os.path.exists('./submission'): os.makedirs('./submission')
        with open(f'submission/out_{self._test_case}', 'w+') as file_obj:
            file_obj.write(f'{objective:.5f} 1\n')
            for c, f in enumerate(assignment):
                file_obj.write(f'{f}\n') if c == len(assignment)-1 \
                    else file_obj.write(f'{f} ')

    def __f_distance(self, fac:Facility, cus:Customer) -> float:
        return np.sqrt((fac.x - cus.x)**2 + (fac.y - cus.y)**2)
    def __get_var_id(self, fid:int, cid:int) -> int:
        return fid*self._M + cid
    def solve(self,
            N:int, M:int,
            sf:List[float], capf:List[float], xf:List[float], yf:List[float],
            dc:List[float], xc:List[float], yc:List[float]) -> None:
        # Build
        self._N, self._M = N, M

        self._facs = []
        for i in range(self._N):
            self._facs.append( self.Facility(float(sf[i]), float(capf[i]), float(xf[i]), float(yf[i])) )

        self._cuss = []
        for i in range(self._M):
            self._cuss.append( self.Customer(float(dc[i]), float(xc[i]), float(yc[i])) )

        # Define the model
        model = LpProblem(name="FacilityLocation", sense=LpMinimize)

        # Decision variables: a[f][c] = bool: Connect facility f -> customer c
        a = {self.__get_var_id(f, c): LpVariable(name=f"a[{f}][{c}]", cat="Binary") \
            for f in range(self._N) for c in range(self._M)}
        # Decision variables: k[f] = bool: Activate facility f
        k = {f: LpVariable(name=f"f[{f}]", cat="Binary") \
            for f in range(self._N)}

        # Constraint: Only able to assign customer to activated facility
        for f in range(self._N):
            for c in range(self._M):
                model += (a[self.__get_var_id(f,c)] <= k[f], f"Assign c[{c}] to [{f}]")

        # Constraint: for each facility, total demand <= cap
        for f in range(self._N):
            f_total_demand = 0
            for c in range(self._M):
                f_total_demand += (a[self.__get_var_id(f,c)]*self._cuss[c].demand)
            model += (f_total_demand - k[f]*self._facs[f].capacity <= 0, f"Total demand <= capacity f[{f}]")

        # Constraint: 1 customer assigned to 1 fac
        for c in range(self._M):
            c_total_fac = 0
            for f in range(self._N):
                c_total_fac += (a[self.__get_var_id(f,c)])
            model += (c_total_fac == 1, f"c[{c}] num fac assigned == 1")

        # Objective: Minimize total cost
        total_cost = 0.
        for f in range(self._N):
            f_distance_cost = 0
            for c in range(self._M):
                f_distance_cost += a[self.__get_var_id(f,c)] * self.__f_distance(self._facs[f], self._cuss[c])
            total_cost += (k[f]*self._facs[f].setup_cost + f_distance_cost)
        model += (total_cost)

        # Solve the optimization problem
        solver = COIN_CMD()
        status = model.solve(COIN_CMD(
            strong=True, cuts=True,
            msg=0, timeLimit=10*60, threads=8, presolve=True))

        if model.status != 1:
            raise f"Can not solve {self._test_case} within time limit"

        # Reconstruct assignment
        assignment = [None] * self._M
        for c in range(self._M):
            for f in range(self._N):
                if a[self.__get_var_id(f,c)].value() > 0:
                    assignment[c] = int(f)
                    break

        # Write ans
        self.__write_best(model.objective.value(), assignment)
