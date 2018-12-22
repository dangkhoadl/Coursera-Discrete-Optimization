

#include <bits/stdc++.h>
using namespace std;

#define ll long long

ll n, K;
vector<ll> val, wei;
int readInput(ifstream &fileIn) {
    fileIn >> n >> K;

    val.assign(n+1, 0);
    wei.assign(n+1, 0);
    for(int i=1; i<=n; ++i) {
        fileIn >> val[i] >> wei[i];
    }

    return 0;
}

// Exact Solution: Dynamic Programming
const int MAXN = 201;
pair<ll, bitset<MAXN>> solveDP() {
    // Init DP and item sets
    vector<vector<ll>> dp(n+1, vector<ll>(K+1, -1));
    vector<vector<bitset<MAXN>>> items(n+1, vector<bitset<MAXN>>(K+1, 0));

    // Base cases
    dp[0][0] = 0;

    // Optimize
    for(int i=0; i<=n; ++i) for(int w=0; w<=K; ++w)  {
        ll cur = dp[i][w];

        if(cur == -1) continue;

        // Add i+1
        if(i+1<=n && w+wei[i+1]<=K) {
            if(dp[i+1][w+wei[i+1]] < cur + val[i+1]) {
                dp[i+1][w+wei[i+1]] = cur + val[i+1];
                items[i+1][w+wei[i+1]] = items[i][w];
                items[i+1][w+wei[i+1]].set(i+1, 1);
            }
        }

        // Not Add i+1
        if(i+1<=n) {
            if(dp[i+1][w] < cur) {
                dp[i+1][w] = cur;
                items[i+1][w] = items[i][w];
            }
        }
    }

    // Get result
    ll res = -1;
    bitset<MAXN> itemSet = 0;
    for(int w=0; w<=K; ++w) {
        if(res < dp[n][w]) {
            res = dp[n][w];
            itemSet = items[n][w];
        }
    }
    return {res, itemSet};
}

// Bounded Solution: Best First Search
const int MAXN_ = 10001;
struct state {
    int id;
    bitset<MAXN_> choice;
    int proc; // 0: not proceed, 1: Choose i+1, 2: Not choose i+1

    ll value;
    ll weight;
    ll estimate;
};
ll calcEstimate(const bitset<MAXN_> &c, int id) {
    ll est=0;
    for(int i=1; i<=id; ++i) if(c[i] == 1) est += val[i];
    for(int i=id+1; i<=n; ++i) est += val[i];
    return est;
}
pair<ll, bitset<MAXN_>> solveBFS() {
    state bestState;

    stack<state> S;
    S.push({
        0, bitset<MAXN_>(0), 0,
        0, 0, accumulate(val.begin(), val.end(), 0)});

    map<pair<int,ll>, ll> DP;
    DP[{0,0}] = 0;

    ll time = 1e7;
    while(!S.empty() && time) {
        state cur = S.top();
        S.pop();

        // pr(cur.id, cur.choice, cur.proc, cur.value, cur.weight, cur.estimate)

        if(cur.proc < 2) S.push({
            cur.id, cur.choice, cur.proc+1,
            cur.value, cur.weight, cur.estimate});


        // End of a branch
        if(cur.id == n && bestState.value < cur.value) {
            bestState = cur;
            // pr(bestState.value, bestState.choice)
            continue;
        }
        
        // Prune
        if(cur.estimate < bestState.value) continue;

        // Proceed next state
        // Choose i+1
        if(cur.proc == 0 && cur.id+1 <= n && cur.weight+wei[cur.id+1] <= K) {
            auto it = DP.find({cur.id + 1, cur.weight + wei[cur.id + 1]});
            if(it == DP.end() || it->second < cur.value + val[cur.id + 1]) {
                DP[{cur.id + 1, cur.weight + wei[cur.id + 1]}] = cur.value + val[cur.id + 1];
                S.push({
                    cur.id + 1, cur.choice.set(cur.id + 1), 0,
                    cur.value + val[cur.id + 1], cur.weight + wei[cur.id + 1], 
                        calcEstimate(cur.choice.set(cur.id + 1), cur.id + 1)});
                --time;
            }
        }

        // Not choose i+1
        else if(cur.proc == 1 && cur.id+1 <= n) {
            auto it = DP.find({cur.id + 1, cur.weight});
            if(it == DP.end() || it->second < cur.value) {
                DP[{cur.id + 1, cur.weight}] = cur.value;
                S.push({
                    cur.id + 1, cur.choice, 0,
                    cur.value, cur.weight, 
                        calcEstimate(cur.choice, cur.id + 1)});
                --time;
            }
        }
    }

    return {bestState.value, bestState.choice};
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
            // Solve
            pair<ll, bitset<MAXN>> results = solveDP();

            // Print results
            cout<< results.first << ' ' << 1 << endl;

            for(int i=1; i<n; ++i) {
                if(results.second[i] == 1) cout << 1 << ' ';
                else cout << 0 << ' ';
            }
            if(results.second[n] == 1) cout << 1 << endl;
            else cout << 0 << endl;
        }

        // Bounded Solution
        else {
            // Solve
            pair<ll, bitset<MAXN_>> results = solveBFS();

            // Print results
            cout<< results.first << ' ' << 0 << endl;

            for(int i=1; i<n; ++i) {
                if(results.second[i] == 1) cout << 1 << ' ';
                else cout << 0 << ' ';
            }
            if(results.second[n] == 1) cout << 1 << endl;
            else cout << 0 << endl;
        }
    }

    return 0;
}
