
#pragma once

// Bounded Solution: Depth First Branch and Bound
#include <bits/stdc++.h>
using namespace std;


class DFBB {
private:
    struct Node {
        int x, y;
    };

    const double inf = 9187201950435737471.0;
    static const int MAXN = 1000 + 2;

    int maxTimeout;
    
    int N;
    vector<Node> nodes;

    unordered_map<bitset<MAXN>,double> dp;
    unordered_map<bitset<MAXN>,vector<int>> orders;
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
    DFBB(const vector<pair<double, double>> &cities, int maxTimeout=1e7) : maxTimeout(maxTimeout) {
        this->N = cities.size();
        this->nodes.assign(this->N, {});
        for(int i=0; i<this->N; ++i)
            this->nodes[i] = {cities[i].first, cities[i].second};
    }
    void solve() {
        // Init DP and ordered sets
        dp.clear();
        orders.clear();
        stack<bitset<MAXN>> S;

        // Base cases - start at city 0
        dp[bitset<MAXN>(1<<0)] = 0.0;
        orders[bitset<MAXN>(1<<0)].push_back(0);
        S.push(bitset<MAXN>(1<<0));

        // best state
        double bestVal = inf;
        vector<int> bestOrder;
        
        // Do Depth First Branch and Bound with timeout
        int timeout = maxTimeout;
        while(!S.empty() && timeout) {
            // current state
            bitset<MAXN> x = S.top();
            double cur = dp[x];
            int lastC = orders[x].back();
            S.pop();

            // Prune
            if(cur > bestVal)
                continue;

            // Proceed next State
            bitset<MAXN> x_;
            double dist_;

            // if traveled to all cities -> back to city 0
            if(x.count() == N) {
                x_ = bitset<MAXN>(1<<N);
                dist_ = cur + dist(nodes[lastC], nodes[0]);
                auto it = dp.find(x_);
                if(dist_ < dp[x_] || it == dp.end()) {
                    // Update best state
                    bestVal = dp[x_] = dist_;
                    bestOrder = orders[x_] = orders[x];
                }
            } 
            // travel to unvisited cities
            else {
                for(int i=0; i<N; ++i) {
                    x_ = x; x_.set(i);
                    dist_ = cur + dist(nodes[lastC], nodes[i]);
                    auto it = dp.find(x_);
                    // Check if i not in x and optimization found
                    if(x[i]==0 && (dist_ < dp[x_] || it == dp.end())) {
                        dp[x_] = dist_;
                        orders[x_] = orders[x]; orders[x_].push_back(i);

                        S.push(x_);
                    }
                }
            }
            --timeout;
        }

        // Print results
        cout << bestVal << ' ' << 0 << endl;
        for(int i=0; i<bestOrder.size()-1; ++i)
            cout << bestOrder[i] << ' ';
        cout << bestOrder[N-1] << endl;
    }
};
