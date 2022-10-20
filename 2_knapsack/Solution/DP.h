#pragma once

/*
Exact Solution: Dynamic Programming
    Time Complexity: O(NK)
*/
class Solution_DP {
private:
    string _test_case;
    int _N, _W;
    const vector<int> &_vals, &_weis;

    void __write_best(int total_val, const bitset<503> &items) {
        ofstream f_out;
        f_out.open("submission/out_" + _test_case);

        f_out << total_val << " 1" << endl;
        for(int i=1; i<=_N; ++i) {
            (items[i] == 0) ? f_out << '0' : f_out << '1';
            (i == _N) ? f_out << endl : f_out << ' ';
        }
        f_out.close();
    }

public:
    Solution_DP(int N, int K, \
            const vector<int>& values, const vector<int>& weights,
            const string &test_case): \
        _N(N), _W(K), _vals(values), _weis(weights), _test_case(test_case) {}

    void solve() {
        // Init
        unordered_map<int, int> dp[2];
        dp[0].clear(); dp[0][0] = 0;
        dp[1].clear(); dp[1][0] = 0;

        unordered_map<int, bitset<503>> items[2];
        items[0].clear(); items[0][0] = 0;
        items[1].clear(); items[0][1] = 0;

        // DP
        int ans_total_val = 0;
        bitset<503> ans_items(0);
        for(int i=0; i<_N; ++i) {
            for(int w=0; w<=_W; ++w) {
                int idx_cur = i % 2;
                int idx_nex = (i+1) % 2;
                if(dp[idx_cur].count(w) == 0) continue;

                // Add i+1
                if(w + _weis[i+1] <= _W) {
                    // Relax next state
                    if(dp[idx_nex][w + _weis[i+1]] < dp[idx_cur][w] + _vals[i+1]) {
                        dp[idx_nex][w + _weis[i+1]] = dp[idx_cur][w] + _vals[i+1];

                        items[idx_nex][w + _weis[i+1]] = items[idx_cur][w];
                        items[idx_nex][w + _weis[i+1]].set(i+1);
                    }

                    // Relax ans
                    if(ans_total_val < dp[idx_nex][w + _weis[i+1]]) {
                        ans_total_val = dp[idx_nex][w + _weis[i+1]];
                        ans_items = items[idx_nex][w + _weis[i+1]];
                    }

                }

                // Not add i + 1, skip
                if(dp[idx_nex][w] < dp[idx_cur][w]) {
                    dp[idx_nex][w] = dp[idx_cur][w];
                    items[idx_nex][w] = items[idx_cur][w];
                }
            }
        }

        // Write ans
        __write_best(ans_total_val, ans_items);
    }
};
