
#pragma once

// Bounded Solution: Depth First Branch and Bound
#include <bits/stdc++.h>
using namespace std;

#define ll long long
static const int MAXN_ = 10003;

struct Object {
    int index;
    ll value;
    ll weight;
};
struct State {
    int id;
    bitset<MAXN_> choice;
    int proc; // 0: not proceed, 1: Choose i+1 alr

    ll value;
    ll weight;
    ll estimate;
};

class DFBB {
private:
    ll n, K;
    vector<ll> val, wei;

    int maxTimeout;
private:
    ll calcEstimate(const bitset<MAXN_> &c, int id, const vector<Object> &objects) {
        ll est=0;
        for(int i=1; i<=id; ++i) 
            if(c[i] == 1) 
                est += objects[i].value;
        for(int i=id+1; i<=n; ++i) 
            est += objects[i].value;
        return est;
    }
public:
    DFBB(ll n, ll K, const vector<ll> &val, const vector<ll> &wei, int maxTimeout) : 
        n(n), K(K), val(val), wei(wei), maxTimeout(maxTimeout) {}
    void solve() {
        // Greedy preprocess (val/wei decreasing)
        vector<Object> objects(n+1);
        for(int i=1; i<=n; ++i) 
            objects[i] = {i, val[i], wei[i]};
        sort(objects.begin()+1, objects.end(), 
            [](const Object &a, const Object &b) {
                return a.value*b.weight > b.value*a.weight;});

        // best State
        State bestState;

        // Stack for Depth First Search
        stack<State> S;
        S.push({
            0, bitset<MAXN_>(0), 0,
            0, 0, accumulate(val.begin()+1, val.end(), 0)});

        // Memorization for ez Pruning
        map<pair<int,ll>, ll> DP;
        DP[{0,0}] = 0;

        // Do Depth First Branch and Bound with timeout
        int timeout = maxTimeout;
        while(!S.empty() && timeout) {
            State cur = S.top();
            S.pop();

            //If do Add i+1 -> Do Not add i+1
            if(cur.proc == 0) 
                S.push({
                    cur.id, cur.choice, 1,
                    cur.value, cur.weight, cur.estimate});

            // End of a branch
            if(cur.id == n && bestState.value < cur.value) {
                bestState = cur;
                continue;
            }
            
            // Prune w/ estimation
            if(cur.estimate < bestState.value)
                continue;

            // Proceed next State
            int nextID;
            ll nextWeight, nextValue;
            bitset<MAXN_> nextChoice;

            // Add id + 1
            nextID = cur.id + 1;
            nextWeight = cur.weight + objects[cur.id + 1].weight;
            nextValue = cur.value + objects[cur.id + 1].value;
            nextChoice = cur.choice; nextChoice.set(cur.id + 1);
            if(cur.proc == 0 && nextID <= n && nextWeight <= K) {
                auto it = DP.find({nextID, nextWeight});
                if(it == DP.end() || it->second < nextValue) {
                    DP[{nextID, nextWeight}] = nextValue;
                    S.push({
                        nextID, nextChoice, 0,
                        nextValue, nextWeight, 
                            calcEstimate(nextChoice, nextID, objects)});
                    --timeout;
                }
            }

            // Not Add id + 1: Do after branching Add id+1
            nextID = cur.id + 1;
            nextWeight = cur.weight;
            nextValue = cur.value;
            nextChoice = cur.choice;
            if(cur.proc == 1 && nextID <= n) {
                auto it = DP.find({nextID, nextWeight});
                if(it == DP.end() || it->second < nextValue) {
                    DP[{nextID, nextWeight}] = nextValue;
                    S.push({
                        nextID, nextChoice, 0,
                        nextValue, nextWeight,
                            calcEstimate(nextChoice, nextID, objects)});
                    --timeout;
                }
            }
        }

        // Rephrase id
        bitset<MAXN_> itemSet(0);
        for(int i=1; i<=n; ++i)
            if(bestState.choice[i] == 1) 
                itemSet[objects[i].index] = 1;
        
        // Print results
        cout<< bestState.value << ' ' << 0 << endl;

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
