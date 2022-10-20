#pragma once

/*
Exact Solution: bitmask Dynamic Programming
    Time Complexity: O(N^2 * 2^N)
*/
class Solution_DP {
private:
    struct Node {
        double x;
        double y;
    };
    typedef unsigned long long ull;

    // Input params
    int _N;
    const vector<double> &_X, &_Y;

    // Main datastructure
    vector<Node> _list_nodes;

private:
    double __f_distance(const Node &a, const Node &b) {
        return sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) );
    }
public:
    Solution_DP(int N, const vector<double> &Xs,const vector<double> &Ys): \
        _N(N), _X(Xs), _Y(Ys) { assert(_X.size() == _Y.size()); }
    void solve() {
        // Build list nodes
        _list_nodes.assign(_N, Node());
        for(int i=0; i<_N; ++i) {
            _list_nodes[i] = {_X[i], _Y[i]};
        }

        // dp[cur_city][visited_subset] = optimal_total_length
        //    if visited_subset == 2^N - 1: visit all cities
        //    if visited_subset == 2^(N+1) - 1: visit all cities and back to the start (complete cycle)
        vector<vector<double>> dp(_N+1, vector<double>((1 << (_N+1)), DBL_MAX));
        for(int i=0; i<_N; ++i) dp[i][1<<i] = 0;

        // Tracing
        vector<vector<int>> pre(_N, vector<int>((1 << _N), -1));
        vector<vector<int>> start_city(_N, vector<int>((1 << _N), -1));
        for(int i=0; i<_N; ++i) start_city[i][1<<i] = i;

        // DP bitmask
        double min_len_tsp = DBL_MAX;
        int last_city = -1;
        for(ull subset=0; subset < (1<<_N+1); ++subset) {
            // loop all subset, travel i -> j
            //  i: in subset
            //  j: j != i and j not in subset
            for(int i=0; i<_N; ++i) if(((1 << i) & subset) > 0) {
                for(int j=0; j<=_N; ++j) if(j != i && (((1 << j) & subset) == 0)) {
                    int subset_add_j = subset | (1 << j);

                    // If reach all cities then back to the start city, complete cycle
                    if(subset==(1<<_N)-1 && subset_add_j==(1<<_N+1)-1) {
                        // Relax
                        int st_city = start_city[i][subset];
                        double dist = __f_distance(_list_nodes[i], _list_nodes[st_city]);
                        if(dp[j][subset_add_j] > dp[i][subset] + dist) {
                            dp[j][subset_add_j] = dp[i][subset] + dist;

                            // Relax ans
                            if(min_len_tsp > dp[j][subset_add_j]) {
                                min_len_tsp = dp[j][subset_add_j];
                                last_city = i;
                            }
                        }
                    }

                    // Else travel i -> j
                    else if(i != _N && j != _N) {
                        // Relax
                        double dist = __f_distance(_list_nodes[i], _list_nodes[j]);
                        if(dp[j][subset_add_j] > dp[i][subset] + dist) {
                            dp[j][subset_add_j] = dp[i][subset] + dist;
                            pre[j][subset_add_j] = i;
                            start_city[j][subset_add_j] = start_city[i][subset];
                        }
                    }
                }
            }
        }

        // Reconstruct
        int cur_city = last_city;
        int subset = (1 << _N) - 1;
        stack<int> path;
        while(cur_city != -1) {
            path.push(cur_city);
            int pre_city = pre[cur_city][subset];

            subset = subset & ~(1 << cur_city);
            cur_city = pre_city;
        }

        // Print ans
        cout << min_len_tsp << " 1" << endl;
        while(!path.empty()) {
            cout << path.top(); path.pop();
            path.size() == 0 ? cout << endl: cout << ' ';
        }
    }
};
