
#pragma once

/* Greedy based on Constraint Programming */

#include <bits/stdc++.h>
using namespace std;

const int MAXCOLOR = 220;


class Greedy {
private:
    struct Node {
        int index;
        int degree;
        int neiColorMin;
    };

    int V, E;
    vector<vector<int>> edges;

    vector<int> deg;
    vector<int> colorMin;
    vector<Node> orders;

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
    void update(int id) {
        deg.assign(V, 0);
        colorMin.assign(V, 999);

        for(int v=0; v<V; ++v) {
            if(color[v] != -1)
                continue;

            unordered_set<int> C;
            for(int i=0; i<edges[v].size(); ++i) {
                int u = edges[v][i];
                
                if(color[u] == -1) 
                    ++deg[v];
                else
                    C.insert(color[u]);
            }
            colorMin[v] = C.size();
        }

        for(int i=id; i<V; ++i) {
            int v = orders[i].index;
            orders[i] = {v, deg[v], colorMin[v]};
        }
    }
    bool paint(int id, int maxColor) {
        // Success found a solution
        if(id == V)
            return true;

        // Greedy every round
        //     - 1st: high neighbor color count
        //     - 2nd: high vertex degree 
        update(id);
        stable_sort(orders.begin() + id, orders.end(),
            [](const Node &a, const Node &b)
                {return a.degree > b.degree;});
        stable_sort(orders.begin() + id, orders.end(),
            [](const Node &a, const Node &b)
                {return a.neiColorMin > b.neiColorMin;});

        // Try to paint each v with minimum max colors
        int v = orders[id].index;
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
    Greedy(int V, int E, const vector<vector<int>> &edges) : 
        V(V), E(E), edges(edges) {}
    void solve() {
        // Reset
        color.assign(V, -1);
        constraints.assign(V, vector<int>(MAXCOLOR, 0));

        orders.assign(V, {});
        for(int v=0; v<V; ++v)
            orders[v] = {v, 0, 0};

        // Solve
        paint(0, MAXCOLOR);

        // Print results
        int colorNum = -1;
        for(int i=0; i<V; ++i)
            colorNum = max(colorNum, color[i]);

        cout << colorNum+1 << ' ' << 0 << endl;
        for(int v=0; v<V-1; ++v)
            cout << color[v] << ' ';
        cout << color[V-1] << endl;
    }
};
