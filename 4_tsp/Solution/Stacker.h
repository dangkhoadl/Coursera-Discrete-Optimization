#pragma once
#include "Heuristics/Heuristic_1side.h"
#include "Heuristics/Heuristic_2side.h"
#include "Local_Search/Hill_Climbing.h"
#include "Local_Search/Simulated_Annealing.h"

class Stacker {
public:
    Stacker() {}
    void solve(int N,
            const vector<double> &Xs,const vector<double> &Ys,
            const string &test_case) {
        assert(Xs.size() == Ys.size());
        cerr << fixed << setprecision(3);

        // Solve
        shared_ptr<Heuristic_1side> heu_1(new Heuristic_1side(
            N, Xs, Ys, 10, test_case));
        auto [heu_1_cost, heu_1_path] = heu_1->solve();
        heu_1.reset();

        cerr << test_case << " : Heuristic 1 : " << heu_1_cost << endl;

        shared_ptr<Simulated_Annealing> sa(new Simulated_Annealing(
            N, Xs, Ys,
            10, test_case));
        auto [sa_cost, sa_path] = sa->solve(heu_1_cost, heu_1_path);
        sa.reset();
        cerr << test_case << " : Simulated Annealing 1: " << sa_cost << endl;

        shared_ptr<Hill_Climbing> hill(new Hill_Climbing(
            N, Xs, Ys,
            10, test_case));
        auto [hill_cost, hill_path] = hill->solve(sa_cost, sa_path);
        hill.reset();
        cerr << test_case << " : Hill Climbing 1: " << hill_cost << endl;
    }
};
