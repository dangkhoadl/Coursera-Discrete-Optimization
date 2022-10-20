from __future__ import annotations
from collections import namedtuple
from typing import List, Tuple, Dict
import numpy as np
from dataclasses import dataclass
import os
from pulp import LpMinimize, LpProblem, LpVariable, LpStatus, COIN_CMD
from sklearn.cluster import KMeans
import copy


import logging
logging.basicConfig(
    format='%(asctime)s : %(name)s : %(levelname)s : %(message)s (%(filename)s:%(lineno)d)',
    level=logging.INFO)
logger = logging.getLogger(__name__)

class MIP_Clustering(object):
    @dataclass
    class Facility(object):
        def __init__(self, idx:int, sf:float, capf: float, xf:float, yf:float) -> None:
            self.idx = idx
            self.setup_cost = sf
            self.capacity = capf
            self.x = xf
            self.y = yf
        def __repr__(self) -> str:
            return f'[idx]:{self.idx}, [sf]:{self.setup_cost}, [cap]:{self.capacity}, [Coordinate]:({self.x},{self.y})'

    @dataclass
    class Customer(object):
        def __init__(self, idx:int, dc:float, xc:float, yc:float) -> None:
            self.idx = idx
            self.demand = dc
            self.x = xc
            self.y = yc
        def __repr__(self) -> str:
            return f'[idx]:{self.idx}, [dc]:{self.demand}, [Coordinate]:({self.x},{self.y})'

    def __init__(self, testcase_f_path) -> None:
        self._N, self._M = None, None
        self._facs = None
        self._cuss = None
        self._test_case = testcase_f_path.split('/')[-1]

    def __write_best(self, objective:float, assignment:List[int]):
        if not os.path.exists('./submission'): os.makedirs('./submission')
        with open(f'submission/out_{self._test_case}', 'w+') as file_obj:
            file_obj.write(f'{objective:.5f} 0\n')
            for c, f in enumerate(assignment):
                file_obj.write(f'{f}\n') if c == len(assignment)-1 \
                    else file_obj.write(f'{f} ')


    def __solve_MIP(self, N:int, facilities:List[Facility], M:int, customers:List[Customer]) -> Tuple[float, Dict[int]]:
        def f_distance(fac:self.Facility, cus:self.Customer) -> float:
            return np.sqrt((fac.x - cus.x)**2 + (fac.y - cus.y)**2)
        def get_var_id(fid:int, cid:int) -> int:
            return fid*M + cid

        # Define the model
        model = LpProblem(name="FacilityLocation", sense=LpMinimize)

        # Decision variables: a[f][c] = bool: Connect facility f -> customer c
        a = {get_var_id(f, c): LpVariable(name=f"a[{f}][{c}]", cat="Binary") \
            for f in range(N) for c in range(M)}
        # Decision variables: k[f] = bool: Activate facility f
        k = {f: LpVariable(name=f"f[{f}]", cat="Binary") \
            for f in range(N)}

        # Constraint: Only able to assign customer to activated facility
        for f in range(N):
            for c in range(M):
                model += (a[get_var_id(f,c)] <= k[f], f"Assign c[{c}] to [{f}]")

        # Constraint: for each f: total demand <= cap
        for f in range(N):
            f_total_demand = 0
            for c in range(M):
                f_total_demand += (a[get_var_id(f,c)]*customers[c].demand)
            model += (f_total_demand - facilities[f].capacity <= 0, f"Total demand <= capacity f[{f}]")

        # Constraint: 1 customer assigned to 1 fac
        for c in range(M):
            c_total_fac = 0
            for f in range(N):
                c_total_fac += (a[get_var_id(f,c)])
            model += (c_total_fac == 1, f"c[{c}] num fac assigned == 1")

        # Objective: Minimize total cost
        total_cost = 0
        for f in range(N):
            distance_cost_f = 0
            for c in range(M):
                distance_cost_f += a[get_var_id(f,c)] * f_distance(facilities[f], customers[c])
            total_cost += (k[f]*facilities[f].setup_cost + distance_cost_f)
        model += (total_cost)

        # Solve the optimization problem
        solver = COIN_CMD()
        status = model.solve(COIN_CMD(
            msg=0, timeLimit=10*60, threads=8, presolve=True))
        logger.info(f"{self._test_case} : facilities size = {N} : customers size = {M} : Status {model.status}, {LpStatus[model.status]} : Objective {model.objective.value():.3f}")

        # Cant solve: return -1
        if model.status != 1:
            return -1, {}

        # Succeed: Return val
        assignment = dict()
        for c, customer in enumerate(customers):
            for f, facility in enumerate(facilities):
                if a[get_var_id(f,c)].value() > 0:
                    assignment[customer.idx] = int(facility.idx)
                    break
        return model.objective.value(), assignment

    def solve(self,
            N:int, M:int,
            sf:List[float], capf:List[float], xf:List[float], yf:List[float],
            dc:List[float], xc:List[float], yc:List[float]) -> None:
        # Build
        self._N, self._M = N, M

        self._facs = []
        for i in range(self._N):
            self._facs.append( self.Facility(i, float(sf[i]), float(capf[i]), float(xf[i]), float(yf[i])) )

        self._cuss = []
        for i in range(self._M):
            self._cuss.append( self.Customer(i, float(dc[i]), float(xc[i]), float(yc[i])) )

        # Scan best cluster size
        best_cost = float("inf")
        best_assign = [-1]*M
        for avg_cluster_size in range(20, 500, 20):
            # K-mean cluster customers
            num_clusters = self._M // avg_cluster_size + 1

            model = KMeans(
                n_clusters=num_clusters, init='random',
                n_init=10, max_iter=300,
                tol=1e-04, random_state=0
            )
            X = np.array([(c.x, c.y) for c in self._cuss])
            y_hat = model.fit_predict(X)
            centroids = model.cluster_centers_

            # Add customers to clusters
            clusters = [ [] for _ in range(len(centroids))]
            for customer in self._cuss:
                clusters[y_hat[customer.idx]].append( customer )

            # Greedy: Prioritise Solving big -> small clusters
            clusters.sort(key=lambda x:len(x), reverse=True)

            # Log cluster info
            logger.info(f"{self._test_case} : avg_cluster_size = {avg_cluster_size} : num_centroids = {len(centroids)}")
            for i, customers in enumerate(clusters):
                logger.info(f"{self._test_case} : centroids[{i}] size {len(customers)}")

            # Solve MIP by cluster
            round_cost = 0.
            round_assign = [-1]*M
            is_succeed = True
            facilities = copy.deepcopy(self._facs)
            for i, customers in enumerate(clusters):
                objective, assignment = self.__solve_MIP(
                    N=self._N, facilities=facilities,
                    M=len(customers), customers=customers)

                if objective == -1:
                    is_succeed = False
                    break

                round_cost += objective

                # Update
                for c, f in assignment.items():
                    round_assign[c] = f
                    facilities[f].setup_cost = 0
                    facilities[f].capacity -= self._cuss[c].demand

            # Update
            if is_succeed:
                logger.info(f"{self._test_case} : avg_cluster_size = {avg_cluster_size} : SUCCEED : round_cost = {round_cost:.3f}")
                if round_cost < best_cost:
                    best_cost = round_cost
                    best_assign = round_assign
                    self.__write_best(best_cost, best_assign)
            else:
                logger.info(f"{self._test_case} : avg_cluster_size = {avg_cluster_size} : FAILED")
                raise f"Can not solve {self._test_case} within time limit"
