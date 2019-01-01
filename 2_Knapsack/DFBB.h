
#pragma once

// Bounded Solution: Depth First Branch and Bound
#include <bits/stdc++.h>
using namespace std;

#define ll long long

class DFBB {
private:
    static const int MAXN = 10000 + 3;
    struct Object {
        int index;
        ll value;
        ll weight;
    };
    struct State {
        bitset<MAXN> choice;
        ll value;
        ll weight;
        ll estimate;
    };

    ll n, K;
    vector<ll> val, wei;
    vector<Object> objects;

    map<pair<int,ll>, ll> DP; // Memorization for ez Pruning
    State bestS, curS;

    ll maxTimeout, timeout;
private:
    ll calcEstimate(const bitset<MAXN> &c, int id) {
        ll est=0;
        for(int i=1; i<=id; ++i) 
            if(c[i] == 1) 
                est += objects[i].value;
        for(int i=id+1; i<=n; ++i)
            est += objects[i].value;
        return est;
    }
    void search(int i) {
        // End of a branch
        if(i == n || timeout < 0) {
            if(bestS.value < curS.value) {
                bestS = curS;
            }
            return;
        }

        for(int c=0; c<=1; ++c) {
            // Add id + 1
            if(c==0) {
                State saveS = curS;

                curS.weight += objects[i+1].weight;
                curS.value += objects[i+1].value;
                curS.choice.set(i+1);
                curS.estimate = calcEstimate(curS.choice, i+1);
                if(i+1 <= n && curS.weight <= K && curS.estimate >= bestS.value) {
                    auto it = DP.find({i+1, curS.weight});
                    if(it == DP.end() || it->second < curS.value) {
                        DP[{i+1, curS.weight}] = curS.value;
                        search(i+1);
                        --timeout;
                    }
                }
                curS = saveS;
            } 

            // Not Add id + 1
            else if(c==1) {
                State saveS = curS;

                if(i+1 <= n && curS.estimate >= bestS.value) {
                    auto it = DP.find({i+1, curS.weight});
                    if(it == DP.end() || it->second < curS.value) {
                        DP[{i+1, curS.weight}] = curS.value;
                        search(i+1);
                        --timeout;
                    }
                }
                curS = saveS;
            }
        }
    }
public:
    DFBB(ll n, ll K, const vector<ll> &val, const vector<ll> &wei, int maxTimeout) : 
        n(n), K(K), val(val), wei(wei), maxTimeout(maxTimeout) {}
    void solve() {
        // Greedy preprocess (val/wei decreasing)
        objects.assign(n+1, {});
        for(int i=1; i<=n; ++i) 
            objects[i] = {i, val[i], wei[i]};
        sort(objects.begin()+1, objects.end(), 
            [](const Object &a, const Object &b) {
                return a.value*b.weight > b.value*a.weight;});

        // Do DFBB
        bestS = {
            bitset<MAXN>(0),
            0, 0, 0};
        curS = {
            bitset<MAXN>(0),
            0, 0, accumulate(val.begin()+1, val.end(), 0)};
        DP.clear();
        timeout = maxTimeout;
        search(0);

        // Rephrase id
        bitset<MAXN> itemSet(0);
        for(int i=1; i<=n; ++i)
            if(bestS.choice[i] == 1) 
                itemSet[objects[i].index] = 1;

        // Print results
        cout<< bestS.value << ' ' << 0 << endl;
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
