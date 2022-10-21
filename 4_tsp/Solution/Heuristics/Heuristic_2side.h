#pragma once
/*
Greedy
    Start with an open path {v_front, ..., v_back}
    while not full cycle:
        u_front = Pick the nearest node to v_front
        u_back = Pick the nearest node to v_back

        if dist(v_front, u_front) < dist(v_back, u_back)
            add u_front to front
        else
            add u_back to back
*/
class Heuristic_2side {
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
    pair<double, vector<int>> __heuristic_search(int starting_v) {
        // Heuristicly construct TSP
        //    Greedy add new node (front or back) to existing path
        deque<int> heu_path; heu_path.push_back(starting_v);
        unordered_set<int> vis({starting_v});
        double heu_cycle_len = 0.0;
        while(true) {
            double dist_back = DBL_MAX;
            double dist_front = DBL_MAX;

            // Exam u closest to the front city
            int v_front = heu_path.front();
            int u_front = -1;
            for(int i=0; i<_N; ++i) {
                if(i != v_front && vis.count(i) == 0 \
                        && dist_front > __f_distance(_A[i], _A[v_front])) {
                    dist_front = __f_distance(_A[i], _A[v_front]);
                    u_front = i;
                }
            }

            // Exam u closest to the back city
            int v_back = heu_path.back();
            int u_back = -1;
            for(int i=0; i<_N; ++i) {
                if(i != v_back && vis.count(i) == 0 \
                        && dist_back > __f_distance(_A[i], _A[v_back])) {
                    dist_back = __f_distance(_A[i], _A[v_back]);
                    u_back = i;
                }
            }

            // Decide to add u to front or back path
            if(dist_front < dist_back) {
                heu_path.push_front(u_front);
                vis.insert(u_front);
                heu_cycle_len += dist_front;
            } else {
                heu_path.push_back(u_back);
                vis.insert(u_back);
                heu_cycle_len += dist_back;
            }

            // If path full, connect front, back to form TSP cycle
            if(vis.size() == _N) {
                heu_cycle_len += __f_distance(_A[heu_path.front()], _A[heu_path.back()]);
                break;
            }
        }
        return {heu_cycle_len, vector<int>(heu_path.begin(), heu_path.end())};
    }
public:
    Heuristic_2side(
            int N,
            const vector<double> &Xs,const vector<double> &Ys,
            int n_trials, const string &test_case): \
        _N(N), _X(Xs), _Y(Ys), _n_trials(n_trials), _test_case(test_case) { assert(_X.size() == _Y.size()); }
    pair<double, vector<int>> solve() {
        // Reseed
        srand(time(0) + rand_int(0, 1e9));

        // Build list nodes
        double x_min = DBL_MAX, x_max = DBL_MIN;
        double y_min = DBL_MAX, y_max = DBL_MIN;
        _A.assign(_N, Node());
        for(int v=0; v<_N; ++v) {
            _A[v] = {_X[v], _Y[v]};

            x_min = min(x_min, _X[v]);
            x_max = min(x_max, _X[v]);

            y_min = min(y_min, _Y[v]);
            y_max = max(y_max, _Y[v]);
        }

        // Pick starting nodes
        unordered_set<int> starting_Vs;
        if(_n_trials < _N) {
            // Manual picks starting nodes
            int candidate = -1;
            double dist;

            // Node closest to x_min, y_min
            dist = DBL_MAX;
            for(int v=0; v<_N; ++v) {
                if(dist > __f_distance(_A[v], Node({x_min, y_min}))) {
                    dist =  __f_distance(_A[v], Node({x_min, y_min}));
                    candidate = v;
                }
            }
            starting_Vs.insert(candidate);

            // Node closest to x_min, y_max
            dist = DBL_MAX;
            for(int v=0; v<_N; ++v) {
                if(dist > __f_distance(_A[v], Node({x_min, y_max}))) {
                    dist =  __f_distance(_A[v], Node({x_min, y_max}));
                    candidate = v;
                }
            }
            starting_Vs.insert(candidate);

            // Node closest to x_max, y_max
            dist = DBL_MAX;
            for(int v=0; v<_N; ++v) {
                if(dist > __f_distance(_A[v], Node({x_max, y_max}))) {
                    dist =  __f_distance(_A[v], Node({x_max, y_max}));
                    candidate = v;
                }
            }
            starting_Vs.insert(candidate);

            // Node closest to x_max, y_min
            dist = DBL_MAX;
            for(int v=0; v<_N; ++v) {
                if(dist > __f_distance(_A[v], Node({x_max, y_min}))) {
                    dist =  __f_distance(_A[v], Node({x_max, y_min}));
                    candidate = v;
                }
            }
            starting_Vs.insert(candidate);

            // random the rest
            while(starting_Vs.size() < _n_trials) {
                starting_Vs.insert( rand_int(0, _N) );
            }

        } else {
            // Try all nodes as starting nodes
            for(int v=0; v<_N; ++v) starting_Vs.insert(v);
        }

        // do heuristic search
        double best_cycle_len = DBL_MAX;
        vector<int> best_path;
        for(const int &v: starting_Vs) {
            auto [heu_cycle_len, heu_path] = __heuristic_search(v);

            // Relax best ans
            if(best_cycle_len > heu_cycle_len) {
                best_cycle_len = heu_cycle_len;
                best_path = heu_path;
                __write_best(best_cycle_len, best_path);
            }
        }

        return {best_cycle_len, best_path};
    }
};
