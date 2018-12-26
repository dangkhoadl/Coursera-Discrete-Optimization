
#pragma once

// Exact Solution: Dynamic Programming
#include <bits/stdc++.h>
using namespace std;

#define ll long long
static const int MAXN = 203;

class DP {
private:
    ll n, K;
    vector<ll> val, wei;
public:
    DP(ll n, ll K, const vector<ll> &val, const vector<ll> &wei) : val(val), wei(wei) {
        this->n = n;
        this->K = K;
    }
    pair<ll, bitset<MAXN>> solve() {
        // Init DP and item sets
        vector<vector<ll>> dp(n+1, vector<ll>(K+1, -1));
        vector<vector<bitset<MAXN>>> items(n+1, vector<bitset<MAXN>>(K+1, 0));

        // Base cases
        dp[0][0] = 0;

        // Optimize
        for(int i=0; i<=n; ++i) for(int w=0; w<=K; ++w) {
            ll cur = dp[i][w];

            if(cur == -1) 
                continue;

            // Add i+1
            if(i+1<=n && w+wei[i+1]<=K) {
                if(dp[i+1][w+wei[i+1]] < cur + val[i+1]) {
                    dp[i+1][w+wei[i+1]] = cur + val[i+1];
                    items[i+1][w+wei[i+1]] = items[i][w]; items[i+1][w+wei[i+1]].set(i+1, 1);
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

        // Print results
        cout<< res << ' ' << 1 << endl;

        for(int i=1; i<n; ++i) {
            if(itemSet[i] == 1)
                cout << 1 << ' ';
            else
                cout << 0 << ' ';
        }
        if(itemSet[n] == 1)
            cout << 1 << endl;
        else
            cout << 0 << endl;
    }
};
