
#pragma once

/* Heuristic based on greedy + Constraint Programming - For dataset V <= 20 */

#include <bits/stdc++.h>
using namespace std;

class Heuristic {
private:
    int V, E;
    vector<vector<int>> edges;

    int searchSpace;
    int maxTimeout, timeout;
    vector<int> color;
    vector<int> searchV;
    vector<pair<int, int>> vDegreeOrder;
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
        if(id == searchV.size())
            return true;

        // Implement timeout
        if(!timeout)
            return false;
        --timeout;

        // Try to paint each v with minimum max colors
        int v = searchV[id];
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
    void propagate(int maxColor, int searchSpace) {
        // Greedy part
        vector<bool> preproceed(V, false);
        for(int i=0; i<vDegreeOrder.size(); ++i) {
            int v = vDegreeOrder[i].first;
            int c = 0;
            while(c < maxColor-searchSpace) {
                if(isFeasible(v, c)) {
                    color[v] = c;
                    preproceed[c] = true;
                    break;
                }
                ++c;
            }
        }

        // Search part
        searchV.clear();
        for(int i=0; i<vDegreeOrder.size(); ++i) {
            int v = vDegreeOrder[i].first;
            if(!preproceed[i])
                searchV.push_back(v);
        }
    }
public:
    Heuristic(int V, int E, const vector<vector<int>> &edges, int searchSpace, int maxTimeout) : 
        V(V), E(E), edges(edges), searchSpace(searchSpace), maxTimeout(maxTimeout) {}
    void solve() {
        // Greedy based on vertex degree(decreasing)
        vDegreeOrder.clear();
        for(int v=0; v<V; ++v)
            vDegreeOrder.push_back({v, edges[v].size()});
        sort(vDegreeOrder.begin(), vDegreeOrder.end(),
            [](const pair<int, int> &a, const pair<int, int> &b)
                {return a.second > b.second;});

        // Find max color
        int maxColor = 1;
        do {
            color.assign(V, -1);
            ++maxColor;
            propagate(maxColor, searchSpace);
            timeout = maxTimeout;
        }
        while(!paint(0, maxColor));

        // Print results
        cout << maxColor << ' ' << 0 << endl;
        for(int v=0; v<V-1; ++v)
            cout << color[v] << ' ';
        cout << color[V-1] << endl;
    }
};
