
#pragma once

// Exact Solution: Dynamic Programming - For number of cities < 25
#include <bits/stdc++.h>
using namespace std;


class DP {
private:
    struct Node {
        int x, y;
    };

    const double inf = 9187201950435737471.0;

    int N;
    vector<Node> nodes;

    vector<double> dp;
    vector<vector<int>> orders;
private:
    bool areSame(double a, double b) {
        return fabs(a-b) < numeric_limits<double>::epsilon();
    }
    double dist(const Node &a, const Node &b) {
        double xx = a.x - b.x;
        double yy = a.y - b.y;
        return sqrt(xx*xx + yy*yy);
    }
public:
    DP(const vector<pair<double, double>> &cities) {
        this->N = cities.size();
        this->nodes.assign(this->N, {});
        for(int i=0; i<this->N; ++i)
            this->nodes[i] = {cities[i].first, cities[i].second};
    }
    void solve() {
        // Init DP and ordered sets
        dp.assign((1<<N) + 1, inf);
        orders.assign((1<<N) + 1, vector<int>());
        queue<int> Q;

        // Base cases - start at city 0
        dp[1<<0] = 0.0;
        orders[1<<0].push_back(0);
        Q.push(1<<0);
        
        // Optimize
        while(!Q.empty()) {
            // current state
            int x = Q.front();
            double cur = dp[x];
            int lastC = orders[x].back();
            Q.pop();

            // skip 
            if(areSame(cur, inf))
                continue;

            // Proceed next State
            int x_;
            double dist_;

            // if traveled to all cities -> back to city 0
            if(x == (1<<N)-1) {
                x_ = 1<<N;
                dist_ = cur + dist(nodes[lastC], nodes[0]);
                if(dist_ < dp[x_]) {
                    dp[x_] = dist_;
                    orders[x_] = orders[x];
                }
            } 
            // travel to unvisited cities
            else {
                for(int i=0; i<N; ++i) {
                    x_ = x | (1<<i);
                    dist_ = cur + dist(nodes[lastC], nodes[i]);
                    // Check if i not in x and optimization found
                    if(!(x&(1<<i)) && dist_ < dp[x_]) {
                        dp[x_] = dist_;
                        orders[x_] = orders[x]; orders[x_].push_back(i);
                        Q.push(x_);
                    }
                }
            }
        }

        // Print results
        cout << dp[1<<N] << ' ' << 1 << endl;
        for(int i=0; i<orders[1<<N].size()-1; ++i)
            cout << orders[1<<N][i] << ' ';
        cout << orders[1<<N][N-1] << endl;
    }
};
