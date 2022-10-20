#include <bits/stdc++.h>
#include <sys/resource.h>
using namespace std;

#define int long long
#include "DP.h"
#include "DFBB.h"


vector<string> split(const string &str, char delim) {
    vector<string> result;
    stringstream str_st(str);
    for (string x(""); getline(str_st, x, (delim));) {
        result.emplace_back(x);
    }
    return move(result);
}

string test_case = "";
int N, K; // num of items, max weight available
vector<int> values, weights;
void read_input(const string &input_fpath) {
    auto parts = split(input_fpath, '/');
    test_case = parts.back();

    ifstream f_in(input_fpath);
    f_in >> N >> K;
    values.assign(N+1, 0);
    weights.assign(N+1, 0);
    for(int i=1; i<=N; ++i) {
        f_in >> values[i] >> weights[i];
    }
    f_in.close();
}

void solve() {
    if(N*K < 5e7) {
        shared_ptr<Solution_DP> dp_solver(new Solution_DP(
            N, K, values, weights, test_case));
        dp_solver->solve();
        dp_solver.reset();
    }
    else {
        shared_ptr<Solution_DFBB> dfbb_solver(new Solution_DFBB(
            N, K, values, weights, 1e10, test_case));
        dfbb_solver->solve();
        dfbb_solver.reset();
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
