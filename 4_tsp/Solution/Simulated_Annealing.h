#pragma once
/*
*/
#include "Heuristics/Heuristic_1side.h"
#include "Heuristics/Heuristic_2side.h"
#include "DEBUG.h"

class Simulated_Annealing {
private:
struct Node {
        double x;
        double y;
    };

    // Input params
    int _N, _n_trials;
    const vector<double> &_X, &_Y;

    // Main data structures
    vector<Node> _A;
private:
    double __f_distance(const Node &a, const Node &b) {
        return sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) );
    }
    double __f_cycle_len(const vector<int> &path) {
        double cycle_len = 0.0;
        for(int i=0; i<_N; ++i) {
            int v = path[i];
            int u = path[(i+1)%_N];
            cycle_len += __f_distance(_A[v], _A[u]);
        }
        return cycle_len;
    }
    vector<int> __swap_edge(int src_1, int src_2,const vector<int> &path) {
        assert(src_1 != src_2);
        vector<int> res;
        // pr(src_1, src_2);
        // pri(path);
        if(src_1 < src_2) {
            int len = src_2 - src_1;
            res.insert(res.end(), path.begin() + src_1 + 1, path.end());
            res.insert(res.end(), path.begin(), path.begin()+ src_1 + 1);
            reverse(res.begin(), res.begin() + len);
        } else {
            int len = src_1 - src_2;
            res.insert(res.end(), path.begin() + src_2 + 1, path.end());
            res.insert(res.end(), path.begin(), path.begin()+ src_2 + 1);
            reverse(res.begin(), res.begin() + len);
        }
        return res;
    }
public:
    Simulated_Annealing(int N, const vector<double> &Xs,const vector<double> &Ys, int n_trials=10): \
        _N(N), _X(Xs), _Y(Ys), _n_trials(n_trials) { assert(_X.size() == _Y.size()); }
    void solve(int n_trials_1, int n_trials_2) {
        // Reseed
        srand(time(0) + rand_int(0, 1e9));

        // Build list nodes
        _A.assign(_N, Node());
        for(int v=0; v<_N; ++v) {
            _A[v] = {_X[v], _Y[v]};
        }

        /// Init Sol
        // Solve Heuristic 1 side
        shared_ptr<Heuristic_1side> heu_1(new Heuristic_1side(
            _N, _X, _Y, n_trials_1));
        auto [cycle_len_1, path_1] = heu_1->solve();
        heu_1.reset();

        // Sovle Heuristic 2 sides
        shared_ptr<Heuristic_2side> heu_2(new Heuristic_2side(
            _N, _X, _Y, n_trials_2));
        auto [cycle_len_2, path_2] = heu_2->solve();
        heu_2.reset();

        // Choose the best solution
        double best_cycle_len = cycle_len_1;
        vector<int> best_path = path_1;
        if(cycle_len_2 < cycle_len_1) {
            best_cycle_len = cycle_len_2;
            best_path = path_2;
        }

        // Simulated Annealing
        double T = 1.0*_N*_N;
        double alpha = 0.75;
        while(_n_trials--) {
            vector<int> cur_path(best_path);
            double cur_cycle_len = best_cycle_len;

            vector<int> new_path;
            double new_cycle_len = 0.0;
            bool accept = false;

            while(!accept) {
                // Exam a neighbor state
                //   Randomly pick 2 edges -> swap
                int src_1 = rand_int(0, _N);
                int src_2 = rand_int(0, _N);
                if(src_1 == src_2 || (src_1+1)%_N == src_2) continue;

                new_path = __swap_edge(src_1, src_2, cur_path);
                new_cycle_len = __f_cycle_len(new_path);

                // if new_cycle_len is better -> accept this stage
                if(new_cycle_len < cur_cycle_len) {
                    accept = true;
                }

                // If new_cycle_len is not better -> setup a probability to accept this stage
                else {
                    double delta = cur_cycle_len - new_cycle_len;
                    double prob = min(1.0, exp(static_cast<double>(delta) / T));
                    accept = rand_prob(prob);
                }
            }

            // Update
            T = max(T*alpha, 0.01);
            cur_path = new_path;
            cur_cycle_len = new_cycle_len;
            if(cur_cycle_len < best_cycle_len) {
                best_cycle_len = cur_cycle_len;
                best_path = cur_path;
            }
        }

        // Print ans
        cout << best_cycle_len << " 0" << endl;
        for(int v=0; v<_N; ++v) {
            cout << best_path[v];
            (v == _N-1) ? cout << endl: cout << ' ';
        }
    }
};
