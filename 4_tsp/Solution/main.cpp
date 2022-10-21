#include <bits/stdc++.h>
#include <sys/resource.h>
#include <omp.h>
#include <unistd.h>
using namespace std;

#define int long long
int rand_int(int a, int b) {
    // Random in range [a, b)
    random_device r;
    default_random_engine e1(r());
    uniform_int_distribution<int> uniform_dist(a, b-1);
    return uniform_dist(e1);
}
bool rand_prob(double prob) {
    // Output: True/False
    // Input: Probability of True
    //      Eg: P(true) = 0.8
    return rand() <  prob * (static_cast<double>(RAND_MAX) + 1.0);
}

#include "DP.h"
#include "Stacker.h"

vector<string> split(const string &str, char delim) {
    vector<string> result;
    stringstream str_st(str);
    for (string x(""); getline(str_st, x, (delim));) {
        result.emplace_back(x);
    }
    return move(result);
}

string test_case;
int N; // num of nodes
vector<double> Xs, Ys;
void read_input(const string &input_fpath) {
    auto parts = split(input_fpath, '/');
    test_case = parts.back();

    ifstream f_in(input_fpath);
    f_in >> N;
    Xs.assign(N, 0.0);
    Ys.assign(N, 0.0);

    for(int i=0; i<N; ++i) {
        f_in >> Xs[i] >> Ys[i];
    }
    f_in.close();
}

void solve() {
    if(N <= 17) {
        shared_ptr<Solution_DP> dp_solver(new Solution_DP(
            N, Xs, Ys, test_case));
        dp_solver->solve();
        dp_solver.reset();
    } else {
        shared_ptr<Stacker> stacker(new Stacker());
        stacker->solve(N, Xs, Ys, test_case);
        stacker.reset();
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
