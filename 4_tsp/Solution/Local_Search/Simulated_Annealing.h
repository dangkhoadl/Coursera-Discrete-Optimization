#pragma once
/*
*/
class Simulated_Annealing {
private:
struct Node {
        double x;
        double y;
    };

    // Input params
    const string &_test_case;
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
    Simulated_Annealing(int N,
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
                __write_best(best_cycle_len, best_path);
            }
        }

        return {best_cycle_len, best_path};
    }
};
