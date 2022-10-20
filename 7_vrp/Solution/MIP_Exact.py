from __future__ import annotations
from typing import List, Tuple
import numpy as np
from pulp import LpMinimize, LpProblem, LpVariable, LpStatus, COIN_CMD
from dataclasses import dataclass
import os

import logging
logging.basicConfig(
    format='%(asctime)s : %(name)s : %(levelname)s : %(message)s (%(filename)s:%(lineno)d)',
    level=logging.INFO)
logger = logging.getLogger(__name__)



class MIP_Exact(object):
    @dataclass
    class Warehouse(object):
        def __init__(self, V:int, C:int, x:float, y:float) -> None:
            self.num_vehicles = V
            self.vehicle_cap = C
            self.x = x
            self.y = y

    @dataclass
    class Customer(object):
        def __init__(self, dc:int, xc:float, yc:float) -> None:
            self.demand = dc
            self.x = xc
            self.y = yc

    def __init__(self, testcase_f_path) -> None:
        self._test_case = testcase_f_path.split('/')[-1]

        self._N, self._V, self._C = None, None, None
        self._warehouse = None
        self._customers = None

    def __write_best(self, objective:float, routes:List[List[int]]):
        if not os.path.exists('./submission'): os.makedirs('./submission')
        with open(f'submission/out_{self._test_case}', 'w+') as file_obj:
            file_obj.write(f'{objective:.5f} 1\n')
            [file_obj.write(' '.join(list(map(str, route))) + '\n') for route in routes]

    def __f_distance(self, src:Customer|Warehouse, dest:Customer|Warehouse) -> float:
        return np.sqrt((src.x - dest.x)**2 + (src.y - dest.y)**2)
    def __get_var_id(self, src_id:int, dest_id:int, ) -> int:
        return src_id*self._N + dest_id

    def solve(self,
            N:int, V:int, C:int,
            dc:List[int], X:List[float], Y:List[float]) -> None:
        # Build
        self._N, self._V, self._C = N, V, C
        self._warehouse = self.Warehouse(int(V), int(C), float(X[0]), float(Y[0]))
        self._customers = [self.Customer(0, self._warehouse.x, self._warehouse.y)] # warehouse
        for i in range(1, self._N):
            self._customers.append( self.Customer(int(dc[i]), float(X[i]), float(Y[i])) )

        # Define the model
        model = LpProblem(name="VehicleRouting", sense=LpMinimize)

        # Decision variables: a[c1][c2] = bool: Connect customer 1 -> customer 2 (include warehouse)
        a = {self.__get_var_id(c1, c2): LpVariable(name=f"a[{c1}][{c2}]", cat="Binary") \
            for c1 in range(self._N) for c2 in range(self._N) if c1 != c2}
        # Decision variable: f[c1][c2] = integer: number(flow) of goods carried by a truck traveling on rout 1->2 (include warehouse)
        f = {self.__get_var_id(c1, c2): LpVariable(name=f"f[{c1}][{c2}]", cat="Integer", lowBound=0) \
            for c1 in range(self._N) for c2 in range(self._N) if c1 != c2}

        # Constraint: total outward edge of a customer = 1
        for c1 in range(1, self._N):
            total_outward_c1 = 0
            for c2 in range(self._N):
                if c1 != c2:
                    total_outward_c1 += a[self.__get_var_id(c1, c2)]
            model += (total_outward_c1 == 1, f"Total outward edge c{c1} == 1")
        # Constraint: total inward edge of a customer = 1
        for c1 in range(1, self._N):
            total_inward_c1 = 0
            for c2 in range(self._N):
                if c1 != c2:
                    total_inward_c1 += a[self.__get_var_id(c2, c1)]
            model += (total_inward_c1 == 1, f"Total inward edge c{c1} == 1")

        # Constraint: total outward edge from the warehouse not exceed number of truck
        total_w_outward = 0
        for c in range(1, self._N):
            total_w_outward += a[self.__get_var_id(0, c)]
        model += (total_w_outward <= self._V, f"Total warehouse outward edge <= num of vehicle {self._V}")
        # Constraint: total inward edge to the warehouse not exceed number of truck
        total_w_inward = 0
        for c in range(1, self._N):
            total_w_inward += a[self.__get_var_id(c, 0)]
        model += (total_w_inward <= self._V, f"Total warehouse inward edge <= num of vehicle {self._V}")

        # Constraint: The goods deliver to customer c1 = demand[c1]
        for c1 in range(1, self._N):
            total_goods_in = 0
            total_goods_out = 0
            for c2 in range(self._N):
                if c1 != c2:
                    total_goods_in += f[self.__get_var_id(c2, c1)]
                    total_goods_out += f[self.__get_var_id(c1, c2)]
            model += (total_goods_out - total_goods_in == self._customers[c1].demand, f"Deliver goods to customer {c1} demand == {self._customers[c1].demand}")

        # Constraint: The goods carried by truck on rout not exceeded the truck capacity
        for c1 in range(self._N):
            for c2 in range(self._N):
                if c1 != c2:
                    model += (f[self.__get_var_id(c1, c2)] <= self._C * a[self.__get_var_id(c1, c2)], f"The goods carried by truck on rout {c1} -> {c2} not exceeded the truck capacity C = {self._C}")

        # Objective: Minimize total cost(distance)
        total_cost = 0.
        for c1 in range(self._N):
            for c2 in range(self._N):
                if c1 != c2:
                    src = self._warehouse if c1 == 0 else self._customers[c1]
                    dest = self._warehouse if c2 == 0 else self._customers[c2]
                    total_cost += a[self.__get_var_id(c1, c2)] * self.__f_distance(src, dest)
        model += (total_cost)

        # Solve the optimization problem
        solver = COIN_CMD()
        status = model.solve(COIN_CMD(
            strong=True, cuts=True,
            msg=0, timeLimit=10*60, threads=8, presolve=True))

        logger.info(f"{self._test_case} : customers size = {self._N-1} : Status {model.status}, {LpStatus[model.status]} : Objective {model.objective.value():.3f}")

        if model.status != 1:
            raise f"Can not solve {self._test_case} within time limit"

        # Reconstruct routes
        routes = [ [0] for _ in range(self._V)]
        start_node_set = set()
        for i in range(self._V):
            # Find strating node
            c1 = -1
            for c in range(1, self._N):
                if a[self.__get_var_id(0, c)].value() > 0 and c not in start_node_set:
                    c1 = c
                    routes[i].append(c1)
                    start_node_set.add(c1)
                    break

            # Early stopping: Got unused vehicles
            if c1 == -1:
                for j in range(i,self._V):
                    routes[j] = [0,0]
                continue

            # Find next node recursively
            c2 = -1
            while c2 != 0:
                for c in range(self._N):
                    if c != c1 and a[self.__get_var_id(c1, c)].value() > 0:
                        c2 = c
                        routes[i].append(c2)
                        c1 = c2
                        break

        # Write ans
        self.__write_best(model.objective.value(), routes)
