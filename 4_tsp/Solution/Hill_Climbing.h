#pragma once
/*
init_solution = Solve 2 heuristic optimizer, pick the best
2-opt Optmization
    Pick 2 edges, swap edge
    if new_cycle_len < cur_cycle_len: update new sol
*/
#include "Heuristics/Heuristic_1side.h"
#include "Heuristics/Heuristic_2side.h"

class Hill_Climbing {
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
    Hill_Climbing(int N, const vector<double> &Xs,const vector<double> &Ys, int n_trials=10): \
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
        vector<int> &best_path = path_1;
        if(cycle_len_2 < cycle_len_1) {
            best_cycle_len = cycle_len_2;
            best_path = path_2;
        }

        // 2-Opt Local search
        while(_n_trials--) {
            // Greedy pick the longest length edge to relax
            // edges[i] = {edge_len, i}
            vector<pair<double, int>> edges;
            for(int i=0; i<_N; ++i) {
                int src = i;
                int dest = (i+1)%_N;
                double edge_len = __f_distance(_A[best_path[src]], _A[best_path[dest]]);
                edges.push_back( {edge_len, i} );
            }
            sort(edges.begin(), edges.end(),
                [] (const pair<double, int> &a, const pair<double, int> &b) \
                    {return a.first > b.first;});

            // Hill Climbing customized
            vector<tuple<int, int, double>> candidates;
            int search_iters = (int)1e6;
            bool searching = true;

            for(int i=0; i<_N-1; ++i) {
                for(int j=i+1; j<_N; ++j) {
                    int src_1 = edges[i].second, dest_1 = (src_1+1)%_N;
                    int src_2 = edges[j].second, dest_2 = (src_2+1)%_N;
                    if(dest_1 == src_2) continue;

                    // Stop searching if too long
                    if(search_iters-- < 0) {
                        searching = false;
                        break;
                    }

                    double new_cycle_len = best_cycle_len \
                        + __f_distance(_A[best_path[src_1]], _A[best_path[src_2]]) \
                        + __f_distance(_A[best_path[dest_1]], _A[best_path[dest_2]]) \
                        - __f_distance(_A[best_path[src_1]], _A[best_path[dest_1]]) \
                        - __f_distance(_A[best_path[src_2]], _A[best_path[dest_2]]);
                    if(new_cycle_len < best_cycle_len) {
                        candidates.push_back( {src_1, src_2, new_cycle_len} );
                    }
                }
                if(!searching) break;
            }
            // Early stopping
            if(candidates.size() == 0) break;

            // Greedy pick the most improvement
            sort(candidates.begin(), candidates.end(),
                [] (const tuple<int, int, double> &a, const tuple<int, int, double> &b) \
                    {return get<2>(a) < get<2>(b);});
            auto [src_1, src_2, new_cycle_len] = candidates[0];

            best_path = __swap_edge(src_1, src_2, best_path);
            best_cycle_len = new_cycle_len;
        }

        // Print ans
        cout << best_cycle_len << " 0" << endl;
        for(int v=0; v<_N; ++v) {
            cout << best_path[v];
            (v == _N-1) ? cout << endl: cout << ' ';
        }
    }
};
