#pragma once
#include "Heuristics.h"
/*
Approx Solution: Heuristic search
*/
class Tabu {
private:
    struct Node {
        int idx;
    };

    struct hash_pair {
        template <class T1, class T2>
        size_t operator()(const pair<T1, T2>& p) const {
            auto hash_1 = hash<T1>{}(p.first);
            auto hash_2 = hash<T2>{}(p.second);
            return hash_1 ^ hash_2;
        }
    };
    // slice in range: [m, n)
    template<typename T>
    vector<T> slice_(const vector<T> &v, int m, int n) {
        assert(m < n && m < v.size());
        auto first = v.cbegin() + m;
        auto last = v.cbegin() + min(n, static_cast<int>(v.size()));
        return vector<T>(first, last);
    }

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
    pair<int, vector<int>> __f_max_color(vector<int> config) {
        // get max color from config
        int max_color = 0;
        vector<int> colors(_V, -1);

        vector<unordered_set<int>> neighbor_colors(_V, unordered_set<int>());
            // neighbor_colors[v]: list of v's neighboring colors

        // Paint all node
        for(int i=0; i<_V; ++i) {
            // Pick v
            int v = config[i];

            // Check neighboring colors
            // Find the correct color for v
            colors[v] = 0;
            while(neighbor_colors[v].count(colors[v]) == 1) colors[v] += 1;

            // Update factors
            for(auto u: _edges[v]) neighbor_colors[u].insert(colors[v]);
            max_color = max(max_color, colors[v]+1);
        }
        return {max_color, colors};
    }

public:
    Tabu(
            int V, int E,
            const vector<vector<int>> &edges,
            int num_trials, const string &test_case): \
        _V(V), _E(E), _edges(edges), _num_trials(num_trials), _test_case(test_case) {}

    void solve() {
        // Reseed
        srand(time(0) + rand_int(0, 1e9));
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();

        // Starting sol Heuristcis
        shared_ptr<Heuristics> heu_solver(new Heuristics(
            _V, _E, _edges, _test_case));
        auto [heu_max_color, heu_colors, heu_config] = heu_solver->solve();
        heu_solver.reset();
        cerr << _test_case << " : Heuristics : " << heu_max_color << endl;

        // Tabu List Local search
        // {max_color, colors, config}
        vector<tuple<int, vector<int>, vector<int>>> candidates;
        candidates.push_back( {heu_max_color, heu_colors, heu_config} );

        set<vector<int>> check_duplicated_config;
        check_duplicated_config.insert( heu_config );

        while(_num_trials--) {
            auto [cand_max_color, cand_colors, cand_config] = candidates[0];

            // params
            int search_iters = (int)1e5;
            vector<int> new_config;
            int i=0, j=0;

            volatile bool flag = false;

            #pragma omp parallel for \
                num_threads(4) \
                private(i, j, new_config) \
                shared(candidates, check_duplicated_config)
            for(int iter=0; iter<search_iters; ++iter) {
                // Shuffle [i,j) segment
                j = rand_int(1, _V);
                i = rand_int(max((int)0, (int)(j-50)), j);
                new_config = cand_config;
                shuffle(new_config.begin()+i, new_config.begin()+j, default_random_engine(seed));

                // test new config
                auto [new_max_color, new_colors] = __f_max_color(new_config);

                #pragma omp critical
                {
                    if(new_max_color < cand_max_color && \
                            check_duplicated_config.count(new_colors) == 0) {
                        candidates.push_back( {new_max_color, new_colors, new_config} );
                        check_duplicated_config.insert( new_config );
                    }
                }
            }

            // Early stopping
            if(candidates.size() == 0) break;

            // Greedy keep the best 5 candidates
            sort(candidates.begin(), candidates.end(),
                [] (const tuple<int, vector<int>, vector<int>> &a, const tuple<int, vector<int>, vector<int>> &b) \
                    {return get<0>(a) < get<0>(b);});
            candidates = slice_<tuple<int, vector<int>, vector<int>>>(candidates, 0, 5);
            // check_duplicated_config.clear();
            // for(auto cand:candidates) check_duplicated_config.insert(get<2>(cand));

            // Record
            auto [best_max_color, best_colors, _] = candidates[0];
            __write_best(best_max_color, best_colors);
            cerr << _test_case << " : Tabu : " << best_max_color << endl;

            // Random next candidate
            int dice = rand_int(0, candidates.size());
            swap(candidates[0], candidates[dice]);
        }
    }
};
