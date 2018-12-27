
#pragma once

/* Exact Solution - Complete method - Constraints Programming - For small dataset */

#include <bits/stdc++.h>
using namespace std;

class CP {
private:
    int V, E;
    vector<vector<int>> edges;

    vector<pair<int, int>> orders;
    vector<int> color;
    vector<vector<int>> constraints;
private:
    void propagate(int v, int c) {
        for(int i=0; i<edges[v].size(); ++i) {
            int u = edges[v][i];
            ++constraints[u][c];
        }
    }
    void remove(int v, int c) {
        for(int i=0; i<edges[v].size(); ++i) {
            int u = edges[v][i];
            if(constraints[u][c] > 0)
                --constraints[u][c];
        }
    }
    bool isFeasible(int v, int c) {
        if(constraints[v][c] > 0)
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
                propagate(v, c);

                // Paint next v
                if(paint(id+1, maxColor))
                    return true;

                // backtrack if failed
                color[v] = -1;
                remove(v, c);
            }
        }
        // Not feasible with maxColor
        return false;
    }
public:
    CP(int V, int E, const vector<vector<int>> &edges) : 
        V(V), E(E), edges(edges) {}
    void solve() {
        // Greedy - high vertex degree
        orders.clear();
        for(int v=0; v<V; ++v) 
            orders.push_back({v, edges[v].size()});
        sort(orders.begin(), orders.end(),
            [](const pair<int, int> &a, const pair<int, int> &b)
                {return a.second > b.second;});

        // Find max color
        int maxColor = 0;
        do {
            ++maxColor;
            color.assign(V, -1);
            constraints.assign(V, vector<int>(maxColor, 0));
        }
        while(!paint(0, maxColor));

        // Print results
        cout << maxColor << ' ' << 1 << endl;
        for(int v=0; v<V-1; ++v)
            cout << color[v] << ' ';
        cout << color[V-1] << endl;
    }
};
