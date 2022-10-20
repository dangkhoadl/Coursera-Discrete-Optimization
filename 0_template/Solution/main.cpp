#include <bits/stdc++.h>
#include <sys/resource.h>
using namespace std;

#define int long long


vector<int> read_input() {
    int N; cin >> N;

    vector<int> inp(N, 0);
    for(int i=0; i<N; ++i) cin >> inp[i];
    return inp;
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
    vector<int> inp = read_input();

    // Solve
    int total = accumulate(inp.begin(), inp.end(), 0);
    cout << total << endl;

    return 0;
}
