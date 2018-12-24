

#include <sys/resource.h>
#include "CP.h"

int V, E;
vector<vector<int>> edges;
void readInput(ifstream &fileIn) {
    fileIn >> V >> E;
    edges.assign(V, vector<int>());

    int v, u;
    for(int i=0; i<E; ++i) {
        fileIn >> v >> u;
        edges[v].push_back(u);
        edges[u].push_back(v);
    }
}


int main(int agrc, char *argv[]) {
    // This code is here to increase the stack size to avoid stack overflow
    // in depth-first search.
    const rlim_t kStackSize = 64L * 1024L * 1024L;  // min stack size = 64 Mb
    struct rlimit rl;
    int32_t result;
    result = getrlimit(RLIMIT_STACK, &rl);
    if(result == 0) {
        if(rl.rlim_cur < kStackSize) {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0) {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    // Program
    if(agrc == 2) {
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

        // Exact Solution - Complete method - For dataset V <= 20
        CP CPsolution(V, E, edges);
        CPsolution.solve();
    }
    return 0;
}
