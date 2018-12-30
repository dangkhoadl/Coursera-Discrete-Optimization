

#include <sys/resource.h>
#include "DP.h"
#include "DFBB.h"

int N;
vector<pair<double, double>> cities;
void readInput(ifstream &fileIn) {
    fileIn >> N;
    cities.assign(N, {});

    double x, y;
    for(int i=0; i<N; ++i) {
        fileIn >> x >> y;
        cities[i] = {x, y};
    }
}

int main(int agrc, char *argv[]) {
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

        // Solve 
        if(N < 600) {
            DFBB DFBBSolution(cities, 1e7);
            DFBBSolution.solve();
        } else {
            cout << 0 << ' ' << 0 << endl;
            for(int i=0; i<N-1; ++i)
                cout << i << ' ';
            cout << N-1 << endl;
        }
    }
    return 0;
}
