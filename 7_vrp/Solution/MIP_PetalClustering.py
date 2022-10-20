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


class MIP_PetalClustering(object):
    @dataclass
    class Warehouse(object):
        def __init__(self, V:int, C:int, x:float, y:float) -> None:
            self.num_vehicles = V
            self.vehicle_cap = C
            self.x = x
            self.y = y

    @dataclass
    class Customer(object):
        def __init__(self, i:int, dc:int, xc:float, yc:float, angle:float) -> None:
            self.idx = i

            self.demand = dc
            self.x = xc
            self.y = yc

            self.orientation = angle
        def __repr__(self) -> str:
            return f'[idx]:{self.idx}, [dc]:{self.demand}, [Coordinate]:({self.x:.2f},{self.y:.2f}), [Orientation]:{self.orientation:.2f}'

    def __init__(self, testcase_f_path) -> None:
        self._test_case = testcase_f_path.split('/')[-1]

        self._N, self._V, self._C = None, None, None
        self._warehouse = None
        self._customers = None

    def __write_best(self, objective:float, routes:List[List[int]]):
        if not os.path.exists('./submission'): os.makedirs('./submission')
        with open(f'submission/out_{self._test_case}', 'w+') as file_obj:
            file_obj.write(f'{objective:.5f} 0\n')
            [file_obj.write(' '.join(list(map(str, route))) + '\n') for route in routes]

    def __f_angle_360(self, x:Tuple[float,float], origin:Tuple[float,float]=(0,0)):
        '''Angle between point x against origin
            Return [0,360]'''
        rad = lambda x: np.arctan2(x[1] - origin[0], x[0] - origin[1])
        return (rad(x) * 180 / np.pi + 360)%360

    def __solve_MIP_miniVRP(self, N:int, V:int, warehouse:Warehouse, customers:Customer):
        def f_distance(src:self.Customer|self.Warehouse, dest:self.Customer|self.Warehouse) -> float:
            return np.sqrt((src.x - dest.x)**2 + (src.y - dest.y)**2)
        def get_var_id(src_id:int, dest_id:int) -> int:
            return src_id*N + dest_id

        # Define the model
        model = LpProblem(name="VehicleRouting", sense=LpMinimize)

        # Decision variables: a[c1][c2] = bool: Connect customer 1 -> customer 2 (include warehouse)
        a = {get_var_id(c1, c2): LpVariable(name=f"a[{c1}][{c2}]", cat="Binary") \
            for c1 in range(N) for c2 in range(N) if c1 != c2}
        # Decision variable: f[c1][c2] = integer: number(flow) of goods carried by a truck traveling on rout 1->2 (include warehouse)
        f = {get_var_id(c1, c2): LpVariable(name=f"f[{c1}][{c2}]", cat="Integer", lowBound=0) \
            for c1 in range(N) for c2 in range(N) if c1 != c2}

        # Constraint: total outward edge of a customer = 1
        for c1 in range(1, N):
            total_outward_c1 = 0
            for c2 in range(N):
                if c1 != c2:
                    total_outward_c1 += a[get_var_id(c1, c2)]
            model += (total_outward_c1 == 1, f"Total outward edge c{c1} == 1")
        # Constraint: total inward edge of a customer = 1
        for c1 in range(1, N):
            total_inward_c1 = 0
            for c2 in range(N):
                if c1 != c2:
                    total_inward_c1 += a[get_var_id(c2, c1)]
            model += (total_inward_c1 == 1, f"Total inward edge c{c1} == 1")

        # Constraint: total outward edge from the warehouse == number of truck
        total_w_outward = 0
        for c in range(1, N):
            total_w_outward += a[get_var_id(0, c)]
        model += (total_w_outward == V, f"Total warehouse outward edge == {V}")
        # Constraint: total inward edge to the warehouse == number of truck
        total_w_inward = 0
        for c in range(1, N):
            total_w_inward += a[get_var_id(c, 0)]
        model += (total_w_inward == V, f"Total warehouse inward edge == {V}")

        # Constraint: The goods deliver to customer c1 = demand[c1]
        for c1 in range(1, N):
            total_goods_in = 0
            total_goods_out = 0
            for c2 in range(N):
                if c1 != c2:
                    total_goods_in += f[get_var_id(c2, c1)]
                    total_goods_out += f[get_var_id(c1, c2)]
            model += (total_goods_out - total_goods_in == customers[c1].demand, f"Deliver goods to customer {c1} demand == {customers[c1].demand}")

        # Constraint: The goods carried by truck on rout not exceeded the truck capacity
        for c1 in range(N):
            for c2 in range(N):
                if c1 != c2:
                    model += (f[get_var_id(c1, c2)] <= warehouse.vehicle_cap * a[get_var_id(c1, c2)], f"The goods carried by truck on rout {c1} -> {c2} not exceeded the truck capacity C = {warehouse.vehicle_cap}")

        # Objective: Minimize total cost(distance)
        total_cost = 0.
        for c1 in range(N):
            for c2 in range(N):
                if c1 != c2:
                    src = warehouse if c1 == 0 else customers[c1]
                    dest = warehouse if c2 == 0 else customers[c2]
                    total_cost += a[get_var_id(c1, c2)] * f_distance(src, dest)
        model += (total_cost)

        # Solve the optimization problem
        solver = COIN_CMD()
        status = model.solve(COIN_CMD(
            strong=True, cuts=True,
            msg=0, timeLimit=30*60, threads=8, presolve=True))

        if model.status != 1:
            return -1, []

        # Reconstruct routes
        routes = [[0] for _ in range(V)]
        start_node_set = set()
        for i in range(V):
            # Find strating node
            c1 = -1
            for c in range(1, N):
                if a[get_var_id(0, c)].value() > 0 and c not in start_node_set:
                    c1 = c
                    routes[i].append(customers[c1].idx)
                    start_node_set.add(c1)
                    break

            # Find next node recursively
            c2 = -1
            while c2 != 0:
                for c in range(N):
                    if c != c1 and a[get_var_id(c1, c)].value() > 0:
                        c2 = c
                        routes[i].append(customers[c2].idx)
                        c1 = c2
                        break
        return model.objective.value(), routes

    def solve(self,
            N:int, V:int, C:int,
            dc:List[int], X:List[float], Y:List[float]) -> None:
        # Build
        self._N, self._V, self._C = N, V, C
        self._warehouse = self.Warehouse(int(V), int(C), float(X[0]), float(Y[0]))
        self._customers = []
        # Calculate the orientation of customer i vs warehouse
        for i in range(1, self._N):
            self._customers.append( self.Customer(
                i, int(dc[i]), float(X[i]), float(Y[i]),
                self.__f_angle_360(
                    x=(float(X[i]), float(Y[i])),
                    origin=(self._warehouse.x, self._warehouse.y))) )

        ## Petal Clustering
        # Sort customer by angular orientation
        self._customers.sort(key=lambda c: c.orientation)
        # Find the optimal split
        max_angular_dist = 0.0
        for i in range(self._N - 1):
            j = (i + 1) % (self._N - 1)
            angular_dist = abs(self._customers[j].orientation - self._customers[i].orientation)
            if max_angular_dist < angular_dist:
                max_angular_dist = angular_dist
                split_pt = j
        self._customers = self._customers[split_pt:] + self._customers[:split_pt]

        # Create Grid search
        #       Prior: petal size dec, num_vehicle dec
        from itertools import product
        MAX_PETAL_SIZE_GRID = min(60, self._N - 1)
        MAX_VEHICLE_SIZE_GRID = min(5, self._V)

        params = [(max_petal_size, max_vehicle_size) for max_petal_size, max_vehicle_size in \
            product(range(MAX_PETAL_SIZE_GRID-20, MAX_PETAL_SIZE_GRID+1, 5), range(2, MAX_VEHICLE_SIZE_GRID+1))]
        params.sort(key=lambda p: p[1], reverse=True)
        params.sort(key=lambda p: p[0], reverse=True)

        best_cost = float("inf")
        for MAX_PETAL_SIZE, MAX_VEHICLE_SIZE in params:
            st_, en_ = 0, MAX_PETAL_SIZE
            num_vehicles = 1
            total_cost, routes = 0., []
            accept = True
            vehicle_used = 0

            # Solve each petal Greedily: increase v num, dec petal size
            while st_ < self._N - 1:
                customers = [self.Customer(0, 0, self._warehouse.x, self._warehouse.y, 0.0)] + self._customers[st_:en_]
                V = min(num_vehicles, self._V - len(routes))

                logger.info(f"{self._test_case} : petal = [{st_}, {en_}) : num vehicles = {V} : vehicles used = {vehicle_used}/{self._V} : solving")
                petal_cost_, routes_ = self.__solve_MIP_miniVRP(
                    N=len(customers), V=V,
                    warehouse=self._warehouse, customers=customers)

                if petal_cost_ == -1:
                    logger.info(f"{self._test_case} : petal = [{st_}, {en_}) : num vehicles = {V} : vehicles used = {vehicle_used}/{self._V} : failed")

                    # If failed increase num_vehicles then decrease petal size
                    if num_vehicles == MAX_VEHICLE_SIZE:
                        en_ -= 1
                        num_vehicles = 1
                    else:
                        num_vehicles = min(num_vehicles + 1, MAX_VEHICLE_SIZE)
                else:
                    # If succeed, move to next petal
                    total_cost += petal_cost_
                    routes += routes_
                    vehicle_used = len(routes)

                    # Log
                    status = 'failed' if vehicle_used >= self._V and en_ < (self._N - 1) else 'ok'
                    logger.info(f"{self._test_case} : petal = [{st_}, {en_}) : petal_cost = {petal_cost_:.3f} : num vehicles = {V} : vehicles used = {vehicle_used}/{self._V} : {status}")

                    st_ = en_
                    en_ = min(st_ + MAX_PETAL_SIZE, self._N-1)
                    num_vehicles = 1


                    # Early stopping check
                    if vehicle_used >= self._V and st_ < (self._N - 1):
                        accept = False
                        break
                    if st_ == self._N-1:
                        for i in range(vehicle_used, self._V):
                            routes.append([0,0])
                        break

            # Check best value
            res = f"total_cost = {total_cost:.3f}) : ACCEPT" if accept else "REJECT"
            logger.info(f"{self._test_case} : {res}")
            if total_cost < best_cost and accept:
                best_cost = total_cost
                self.__write_best(best_cost, routes)
