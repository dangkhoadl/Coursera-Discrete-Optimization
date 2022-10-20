#pragma once
/*
Approx Solution: Heuristic search
*/

int rand_int(int a, int b) {
    random_device r;
    default_random_engine e1(r());
    uniform_int_distribution<int> uniform_dist(a, b-1);
    return uniform_dist(e1);
}

class Solution_Heuristic {
private:
    struct Node {
        int idx;
        int degree;
    };

    // Input params
    string _test_case;
    int _V, _E, _num_trials;
    const vector<vector<int>> &_edges;

private:
    void __write_best(int best_max_color, const vector<int> &best_color) {
        ofstream f_out;
        f_out.open("submission/out_" + _test_case);

        f_out << best_max_color << " 0" << endl;
        for(int v=0; v<_V; ++v) {
            f_out << best_color[v];
            (v == _V-1) ? f_out << endl : f_out << ' ';
        }
        f_out.close();
    }

    pair<int, vector<int>> __heuristic_search(vector<Node> &A) {
        int heu_max_color = 0;
        vector<int> color(_V, -1);

        vector<int> painted_neighbors(_V, 0);
            // painted_neighbors[v]: number of v's neighboring nodeshave been painted
        vector<unordered_set<int>> neighbor_colors(_V, unordered_set<int>());
            // neighbor_colors[v]: list of v's neighboring colors
        for(int i=0; i<_V; ++i) {
            /* Greedy pick v with the highest
                f = degree * (painted neighbors+1) * (neighbor_colors+1)
            */
            auto f = [&](const Node &a, const Node &b) -> bool {
                return a.degree*(painted_neighbors[a.idx] + 1)*(neighbor_colors[a.idx].size()+1) > \
                    b.degree*(painted_neighbors[b.idx]+1)*(neighbor_colors[b.idx].size()+1);};
            sort(A.begin() + i, A.end(), f);

            // Random factor: shuffle the first 30 candidates
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(A.begin()+i, A.begin() + min(i+30, _V), default_random_engine(seed));

            // Pick v
            int v = A[i].idx;

            // Check neighboring colors
            // Find the correct color for v
            color[v] = 0;
            while(neighbor_colors[v].count(color[v]) == 1) color[v] += 1;

            // Update factors
            for(auto u: _edges[v]) {
                painted_neighbors[u] += 1;
                neighbor_colors[u].insert(color[v]);
            }
            heu_max_color = max(heu_max_color, color[v]+1);
        }
        return {heu_max_color, color};
    }
public:
    Solution_Heuristic(
            int V, int E,
            const vector<vector<int>> &edges,
            int num_trials, const string &test_case): \
        _V(V), _E(E), _edges(edges), _num_trials(num_trials), _test_case(test_case) {}

    void solve() {
        // Reseed
        srand(time(0) + rand_int(0, 1e9));

        // Build list of nodes
        vector<Node> A(_V, Node());
        for(int v=0; v<_V; ++v) {
            A[v] = {v, (int)_edges[v].size()} ;
        }

        // Heuristic painting, try multiple times
        int best_max_color = _V;
        for(int trial=0; trial<_num_trials; ++trial) {
            auto [heu_max_color, heu_color] = __heuristic_search(A);

            if(best_max_color > heu_max_color) {
                best_max_color = heu_max_color;
                __write_best(best_max_color, heu_color);
            }
        }
    }
};
