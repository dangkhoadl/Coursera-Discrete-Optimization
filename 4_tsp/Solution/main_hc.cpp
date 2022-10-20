#include <bits/stdc++.h>
#include <sys/resource.h>
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
#include "Hill_Climbing.h"
#include "Simulated_Annealing.h"

// num of noders, num of edges
int N;
vector<double> Xs, Ys;
void read_input() {
    cin >> N;
    Xs.assign(N, 0.0);
    Ys.assign(N, 0.0);

    for(int i=0; i<N; ++i) {
        cin >> Xs[i] >> Ys[i];
    }
}

void solve() {
    // Exact solution
    if(N <= 17) {
        shared_ptr<Solution_DP> dp_solver(new Solution_DP(
            N, Xs, Ys));
        dp_solver->solve();
        dp_solver.reset();
    }
    else if(N <= 1000) {
        shared_ptr<Hill_Climbing> lc_search(new Hill_Climbing(
            N, Xs, Ys, N*N));
        lc_search->solve(N*N, N*N);
        lc_search.reset();
    }
    else if(N <= 2000) {
        shared_ptr<Hill_Climbing> lc_search(new Hill_Climbing(
            N, Xs, Ys, 11*N));
        lc_search->solve(9*N, 9*N);
        lc_search.reset();
    } else {
        shared_ptr<Hill_Climbing> lc_search(new Hill_Climbing(
            N, Xs, Ys, 20));
        lc_search->solve(20, 20);
        lc_search.reset();
    }
}


int32_t main() {
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

    // Read stdin
    read_input();

    // Solve
    cout << fixed << setprecision(3);
    solve();

    return 0;
}
