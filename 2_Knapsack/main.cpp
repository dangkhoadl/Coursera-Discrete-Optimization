
#include <sys/resource.h>
#include "DP.h"
#include "DFBB.h"

ll n, K;
vector<ll> val, wei;
int readInput(ifstream &fileIn) {
    fileIn >> n >> K;

    val.assign(n+1, 0);
    wei.assign(n+1, 0);
    for(int i=1; i<=n; ++i) fileIn >> val[i] >> wei[i];

    return 0;
}


// Cmd: ./a.out tmp.data
int main(int agrc, char *argv[]) {
    // This code is here to increase the stack size to avoid stack overflow
    // in depth-first search.
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

    // Program
    if (agrc == 2) {
        // Get input file (tmp.data)
        const string inputFile(argv[1]);
        ifstream fileIn;
        fileIn.open(inputFile);
        if (!fileIn) {
            cout << "File does not exist !!!" << endl;
            return 0;
        }

        // Reads input data
        readInput(fileIn);

        // Exact Solution
        if(n*K < (ll)5e7) {
            DP DPsolution(n, K, val, wei);
            DPsolution.solve();
        }
        // Bounded Solution
        else if(n*K < (ll)1e10) {
            DFBB DFBBsolution(n, K, val, wei, 1e7);
            DFBBsolution.solve();
        } 
        else {
            DFBB DFBBsolution(n, K, val, wei, 1e8);
            DFBBsolution.solve();
        }
    }

    return 0;
}
