
#pragma once

/* Exact Solution - Complete method - Constraints Programming - For small dataset */

#include <bits/stdc++.h>
using namespace std;

class Brute {
private:
    int V, E;
    vector<vector<int>> edges;

    vector<int> color;
    vector<pair<int, int>> orders;
private:
    bool isFeasible(int v, int c) {
        // Check if same color w/ neighbors
        for(int i=0; i<edges[v].size(); ++i)
            if(color[edges[v][i]] == c)
                return false;
        return true;
    }
    bool paint(int id, int maxColor) {
        // Success found a solution
        if(id == V)
            return true;

        // Try to paint each v with minimum max colors
        int v = orders[id].first;
        for(int c=0; c<maxColor; ++c) {
            if(isFeasible(v, c)) {
                // Paint v
                color[v] = c;

                // Paint next v
                if(paint(id+1, maxColor))
                    return true;

                // backtrack if failed
                color[v] = -1;
            }
        }
        // Not feasible with maxColor
        return false;
    }
public:
    Brute(int V, int E, const vector<vector<int>> &edges) : 
        V(V), E(E), edges(edges) {}
    void solve() {
        // Find coloring order - Greedy vertex degree
        orders.clear();
        for(int v=0; v<V; ++v) 
            orders.push_back({v, edges[v].size()});
        sort(orders.begin(), orders.end(),
            [](const pair<int, int> &a, const pair<int, int> &b)
                {return a.second > b.second;});

        // Find max color
        int maxColor = 0;
        do {
            color.assign(V, -1);
            ++maxColor;
        }
        while(!paint(0, maxColor));

        // Print results
        cout << maxColor << ' ' << 1 << endl;
        for(int v=0; v<V-1; ++v)
            cout << color[v] << ' ';
        cout << color[V-1] << endl;
    }
};

