#pragma once
/*
*/

class Solution_DFBB {
private:
    // Struct
    struct Object {
        int idx;
        int value;
        int weight;
    };
    struct State {
        int value;
        int weight;
        bitset<10003> items;
    };
    struct hash_pair {
        template <class T1, class T2>
        size_t operator()(const pair<T1, T2>& p) const {
            auto hash_1 = hash<T1>{}(p.first);
            auto hash_2 = hash<T2>{}(p.second);
            return hash_1 ^ hash_2;
        }
    };

    // Input params
    string _test_case;
    int _N, _W, _iters;
    const vector<int> &_vals, &_weis;

    // main datastructures
    vector<Object> _A;

    // Ans, optimal solution
    State _st_best;

    // caches
    vector<int> _prefix_vals;
    unordered_map<pair<int, int>, int, hash_pair> _dp;

private:
    void __write_best(int total_val, const bitset<10003> &items) {
        ofstream f_out;
        f_out.open("submission/out_" + _test_case);

        f_out << total_val << " 0" << endl;
        for(int i=1; i<=_N; ++i) {
            (items[i] == 0) ? f_out << '0' : f_out << '1';
            (i == _N) ? f_out << endl : f_out << ' ';
        }
        f_out.close();
    }

    int __f_estimate(int cur_val, int i) {
        // estimate =
        //  current total value in knapsack + possible total value if adding all [i, N] items
        return cur_val - _prefix_vals[i-1] + _prefix_vals[_N];
    }

    void __dfs(int i, const State &st_cur) {
        if(i == _N || _iters < 0) return;

        State st_nex = {0,0,0};
        int estimate = 0;

        // Decision to add i + 1
        st_nex = st_cur;
        st_nex.value += _A[i+1].value;
        st_nex.weight += _A[i+1].weight;
        st_nex.items.set(_A[i+1].idx);
        estimate = __f_estimate(st_cur.value, i+1);
        if(st_nex.weight <= _W && estimate >= _st_best.value) {
            auto it = _dp.find( {i+1, st_nex.weight} );
            if(it == _dp.end() || it->second < st_nex.value) {
                // Relax
                _iters -= 1;
                _dp[{i+1, st_nex.weight}] = st_nex.value;
                if(_st_best.value < st_nex.value) {
                    _st_best = st_nex;
                    __write_best(_st_best.value, _st_best.items);
                }

                // Next i
                __dfs(i+1, st_nex);
            }
        }

        // Decision to skip i + 1
        st_nex = st_cur;
        estimate = __f_estimate(st_cur.value, i+2);
        if(estimate >= _st_best.value) {
            auto it = _dp.find( {i+1, st_nex.weight} );
            if(it == _dp.end() || it->second < st_nex.value) {
                // Relax
                _iters -= 1;
                _dp[{i+1, st_nex.weight}] = st_nex.value;

                // Next i
                __dfs(i+1, st_nex);
            }
        }
    }
public:
    Solution_DFBB(int N, int K,
        const vector<int>& values, const vector<int>& weights,
        int max_iters, const string &test_case): \
        _N(N), _W(K), _vals(values), _weis(weights), _iters(max_iters), _test_case(test_case) {}

    void solve() {
        // Greedy: Prefer val/wei decreasing
        _A.assign(_N+1, {0,0,0});
        for(int i=1; i<=_N; ++i) {
            _A[i] = {i, _vals[i], _weis[i]};
        }
        sort(_A.begin()+1, _A.end(),
            [] (const Object &a, const Object &b) {
                return a.value*b.weight > b.value*a.weight;});

        // cache prefix val for calculating estimate
        _prefix_vals.assign(_N+1, 0);
        for(int i=0; i<_N; ++i) {
            _prefix_vals[i+1] = _prefix_vals[i] + _A[i+1].value;
        }

        // do DFBB
        _dp.clear();
        _st_best = {0,0,0};
        __dfs(0, {0,0,0});
    }
};
