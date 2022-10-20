#include <bits/stdc++.h>
#include <sys/resource.h>
using namespace std;


#define int long long
int rand_int(int a, int b) {
    random_device r;
    default_random_engine e1(r());
    uniform_int_distribution<int> uniform_dist(a, b-1);
    return uniform_dist(e1);
}
#include "Backtrack.h"
#include "Heuristic.h"

vector<string> split(const string &str, char delim) {
    vector<string> result;
    stringstream str_st(str);
    for (string x(""); getline(str_st, x, (delim));) {
        result.emplace_back(x);
    }
    return move(result);
}

string test_case = "";
int N, E; // num of noders, num of edges
vector<vector<int>> edges;
void read_input(const string &input_fpath) {
    auto parts = split(input_fpath, '/');
    test_case = parts.back();

    ifstream f_in(input_fpath);
    f_in >> N >> E;
    edges.assign(N, vector<int>());

    int u, v;
    for(int i=0; i<E; ++i) {
        f_in >> v >> u;
        edges[v].push_back(u);
        edges[u].push_back(v);
    }
    f_in.close();
}

void solve() {
    // Exact solution
    if(E <= 600) {
        shared_ptr<Solution_Backtrack> bt_solver(new Solution_Backtrack(
            N, E, edges, test_case));
        bt_solver->solve();
        bt_solver.reset();
    } else {
        shared_ptr<Solution_Heuristic> heu_solver(new Solution_Heuristic(
            N, E, edges, 5e8, test_case));
        heu_solver->solve();
        heu_solver.reset();
    }
}


int32_t main(int agrc, char *argv[]) {
    // This code is here to increase the stack size to avoid stack overflow
    //      in depth-first search.
    const rlim_t kStackSize = 64L * 1024L * 1024L;  // min stack size = 64 Mb
    struct rlimit rl;
    int32_t result;
    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0) {
        if (rl.rlim_cur < kStackSize) {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0) {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    if(agrc != 2) {
        cerr << "Format: ./main.exe data/<test-case-fpath>" << endl;
        return 1;
    }

    // Read stdin
    string input_fpath(argv[1]);
    read_input(input_fpath);

    // Solve
    solve();

    return 0;
}
