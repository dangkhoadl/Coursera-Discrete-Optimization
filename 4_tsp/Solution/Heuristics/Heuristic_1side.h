#pragma once
/*
Greedy
    Start with a node v
    while not full cycle:
        u = Pick the nearest node to v
        v = u
*/
class Heuristic_1side {
private:
    struct Node {
        int idx;
        double x;
        double y;

        double distance;
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
    pair<double, vector<int>> __heuristic_search(vector<Node> &A) {
        double heu_cycle_len = 0.0;
        vector<int> heu_path;
        for(int i=0; i<_N; ++i) {
            // Greedy pick u closest to v
            for(int j=i+1; j<_N; ++j) {
                A[j].distance = __f_distance(A[i], A[j]);
            }
            auto comp = [&](const Node &a, const Node &b) -> bool {
                return a.distance < b.distance;};
            sort(A.begin()+i+1, A.end(), comp);

            // Connect v -> u
            // int v = A[i].idx;
            int u = (i == _N-1) ? A[0].idx: A[i+1].idx;
            double dist = (i == _N-1) ? \
                __f_distance(A[i], A[0]): \
                __f_distance(A[i], A[i+1]);

            heu_cycle_len += dist;
            heu_path.push_back(u);
        }
        return {heu_cycle_len, heu_path};
    }
public:
    Heuristic_1side(int N, const vector<double> &Xs,const vector<double> &Ys, int n_trials=10): \
        _N(N), _X(Xs), _Y(Ys), _n_trials(n_trials) { assert(_X.size() == _Y.size()); }
    pair<double, vector<int>> solve() {
        // Reseed
        srand(time(0) + rand_int(0, 1e9));

        // Build list nodes
        double x_min = DBL_MAX, x_max = DBL_MIN;
        double y_min = DBL_MAX, y_max = DBL_MIN;
        _A.assign(_N, Node());
        for(int v=0; v<_N; ++v) {
            _A[v] = {v, _X[v], _Y[v], 0.0};

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
            vector<Node> A_tmp(_A);

            swap(A_tmp[0], A_tmp[v]);
            auto [heu_cycle_len, heu_path] = __heuristic_search(A_tmp);

            // Relax best ans
            if(best_cycle_len > heu_cycle_len) {
                best_cycle_len = heu_cycle_len;
                best_path = heu_path;
            }
        }

        return {best_cycle_len, best_path};
    }
};
