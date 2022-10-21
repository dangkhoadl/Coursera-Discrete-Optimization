#pragma once
/*
init_solution = Solve 2 heuristic optimizer, pick the best
2-opt Optmization
    Pick 2 edges, swap edge
    if new_cycle_len < cur_cycle_len: update new sol
*/
class Hill_Climbing {
private:
struct Node {
        double x;
        double y;
    };

    // Input params
    string _test_case;
    int _N, _n_trials;
    const vector<double> &_X, &_Y;

    // Main data structures
    vector<Node> _A;
private:
    void __write_best(int best_cycle_len, const vector<int> &best_path) {
        ofstream f_out;
        f_out.open("submission/out_" + _test_case);
        f_out << fixed << setprecision(6);

        f_out << best_cycle_len << " 0" << endl;
        for(int v=0; v<_N; ++v) {
            f_out << best_path[v];
            (v == _N-1) ? f_out << endl: f_out << ' ';
        }
        f_out.close();
    }

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
    Hill_Climbing(
            int N,
            const vector<double> &Xs,const vector<double> &Ys,
            int n_trials, const string &test_case):
        _N(N), _X(Xs), _Y(Ys), _n_trials(n_trials), _test_case(test_case) { assert(_X.size() == _Y.size()); }
    pair<double, vector<int>> solve(double init_cycle_len, const vector<int> &init_path) {
        // Reseed
        srand(time(0) + rand_int(0, 1e9));

        // Build list nodes
        _A.assign(_N, Node());
        for(int v=0; v<_N; ++v) {
            _A[v] = {_X[v], _Y[v]};
        }

        // Check Init Sol
        double best_cycle_len = init_cycle_len;
        vector<int> best_path(init_path);

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

            best_cycle_len = new_cycle_len;
            best_path = __swap_edge(src_1, src_2, best_path);
            __write_best(best_cycle_len, best_path);
        }

        return {best_cycle_len, best_path};
    }
};
