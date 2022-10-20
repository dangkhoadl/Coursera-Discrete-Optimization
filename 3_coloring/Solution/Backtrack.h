#pragma once

/*
Exact Solution: Binary search + Backtrack+Constraints+GreedyPick
*/
class Solution_Backtrack {
private:
    struct Node {
        int idx;
        int degree;
    };

    // Input params
    int _V, _E;
    const vector<vector<int>> &_edges;

    // Main datastructure
    string _test_case;
    vector<Node> _A;
    vector<int> _color;
        // _color[v]: color of node v

    // Constraints/Decision making
    vector<vector<int>> _neighboring_colors;
        // _neighboring_colors[v][c]: number of v's neighboring nodes have c color
    vector<int> _painted_neighbors;
        // _painted_neighbors[v]: number of v's neighboring nodeshave been painted

private:
    void __write_best(int best_max_color, const vector<int> &best_color) {
        ofstream f_out;
        f_out.open("submission/out_" + _test_case);

        f_out << best_max_color << " 1" << endl;
        for(int v=0; v<_V; ++v) {
            f_out << best_color[v];
            (v == _V-1) ? f_out << endl : f_out << ' ';
        }
        f_out.close();
    }

    bool __is_feasible(int v, int c) {
        return (_color[v] == -1) && (_neighboring_colors[v][c] == 0);
    }
    bool __dfs(int i, int max_color) {
        if(i == _V) return true;

        // Greedy pick v has highest (degree * painted neighbors) ratio
        auto comp = [&](const Node &a, const Node &b) -> bool {
            return a.degree*_painted_neighbors[a.idx] > b.degree*_painted_neighbors[b.idx];};
        sort(_A.begin() + i, _A.end(), comp);
        int v = _A[i].idx;

        // Try to paint v
        for(int c=0; c<max_color; ++c) {
            if(__is_feasible(v, c)) {
                // Paint v with c
                _color[v] = c;
                for(auto &u: _edges[v]) {
                    _neighboring_colors[u][c] += 1;
                    _painted_neighbors[u] += 1;
                }

                if(__dfs(i+1, max_color)) return true;

                // backtrack
                _color[v] = -1;
                for(auto &u: _edges[v]) {
                    _neighboring_colors[u][c] -= 1;
                    _painted_neighbors[u] -= 1;
                }
            }
        }
        return false;
    }
public:
    Solution_Backtrack(int V, int E, const vector<vector<int>> &edges, const string &test_case): \
        _V(V), _E(E), _edges(edges), _test_case(test_case) {}

    void solve() {
        // Build list of node
        _A.assign(_V, {});
        for(int v=0; v<_V; ++v) {
            _A[v] = {v, (int)_edges[v].size()};
        }

        // Binary search for best max_color
        int l = 1;
        int r = _V;
        int best_max_color = _V;
        while(l <= r) {
            int max_color = l + (r-l) / 2;

            // Reset
            _color.assign(_V, -1);
            _neighboring_colors.assign(_V, vector<int>(max_color, 0));
            _painted_neighbors.assign(_V, 1);

            // Try if max_color feasible
            if(__dfs(0, max_color)) {
                r = max_color - 1;

                // Relax ans
                if(best_max_color > max_color) {
                    best_max_color = max_color;
                    __write_best(best_max_color, _color);
                }
            } else {
                l = max_color + 1;
            }
        }
    }
};
