

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

        // Split solutions
        const ll constraint = 5e7 + 1;

        // Exact Solution
        if(n*K < constraint) {
            DP DPsolution(n, K, val, wei);
            DPsolution.solve();
        }
        // Bounded Solution
        else {
            DFBB DFBBsolution(n, K, val, wei);
            DFBBsolution.solve();
        }
    }

    return 0;
}
