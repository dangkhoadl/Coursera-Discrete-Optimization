
#pragma once

/* Heuristic based on greedy + Constraint Programming */

#include <bits/stdc++.h>
using namespace std;

class Greedy {
private:
    int V, E;
    vector<vector<int>> edges;

    int searchSpace;
    int maxTimeout, timeout;

    vector<int> color;
    vector<vector<int>> constraints;

    vector<pair<int, int>> vDegreeOrder;
    vector<pair<int, int>> vConstraintOrder;
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
    void split(int maxColor, int searchSpace) {
        // Greedy part
        vector<bool> preproceed(V, false);
        for(int i=0; i<vDegreeOrder.size(); ++i) {
            int v = vDegreeOrder[i].first;
            int c = 0;
            while(c < maxColor-searchSpace) {
                if(isFeasible(v, c)) {
                    color[v] = c;
                    propagate(v, c);
                    preproceed[c] = true;
                    break;
                }
                ++c;
           }
        }

        // Search part
        vConstraintOrder.clear();
        for(int i=0; i<vDegreeOrder.size(); ++i) {
            int v = vDegreeOrder[i].first;
            if(!preproceed[i]) {
                int cnt = 0;
                for(int c=0; c<maxColor; ++c)
                    if(constraints[v][c] == 0)
                        ++cnt;
                vConstraintOrder.push_back({v, cnt});
            }
        }
    }
    bool isFeasible(int v, int c) {
        if(constraints[v][c] > 0)
            return false;
        return true;
    }
    bool paint(int id, int maxColor) {
        // Success found a solution
        if(id == vConstraintOrder.size())
            return true;

        // Implement timeout
        if(!timeout)
            return false;
        --timeout;

        // Try to paint each v with minimum max colors
        int v = vConstraintOrder[id].first;
        for(int c=0; c<maxColor; ++c) {
            if(isFeasible(v, c)) {
                // Paint v
                color[v] = c;
                propagate(v, c);

                // Paint next v
                if(paint(id+1, maxColor))
                    return true;

                // Backtrack if failed
                color[v] = -1;
                remove(v, c);
            }
        }
        // Not feasible with maxColor
        return false;
    }
public:
    Greedy(int V, int E, const vector<vector<int>> &edges, int searchSpace, int maxTimeout) : 
        V(V), E(E), edges(edges), searchSpace(searchSpace), maxTimeout(maxTimeout) {}
    void solve() {
        // Greedy 1 - high vertex degree
        vDegreeOrder.clear();
        for(int v=0; v<V; ++v)
            vDegreeOrder.push_back({v, edges[v].size()});
        sort(vDegreeOrder.begin(), vDegreeOrder.end(),
            [](const pair<int, int> &a, const pair<int, int> &b)
                {return a.second > b.second;});

        // Find max color
        int maxColor = 0;
        do {
            ++maxColor;
            color.assign(V, -1);
            constraints.assign(V, vector<int>(maxColor, 0));
            timeout = maxTimeout;
            split(maxColor, searchSpace);

            // Greedy 2 - low vertex constraints
            sort(vConstraintOrder.begin(), vConstraintOrder.end(),
                [](const pair<int, int> &a, const pair<int, int> &b)
                    {return a.second < b.second;});
        }
        while(!paint(0, maxColor));

        // Print results
        cout << maxColor << ' ' << 0 << endl;
        for(int v=0; v<V-1; ++v)
            cout << color[v] << ' ';
        cout << color[V-1] << endl;
    }
};
