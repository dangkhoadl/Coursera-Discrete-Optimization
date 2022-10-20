#pragma once

#include <bits/stdc++.h>
using namespace std;

/*------------------------------------- CONFIG ---------------------------------------------------------------*/
// #define QUICK_TYPING

/*------------------------------------- Function list ------------------------------------------------------
#define quit
#define nl
#define pr(args...)
#define pra(A,start,end)
#define pri(A)
#define pri_(A)
#define prkv(A)
#define praa(A,start1,end1,start2,end2)
#define prii(A)
template<typename T> void prS(const stack<T> &fdfsfv_a) {}
template<typename T> void prQ(const queue<T> &fdfsfv_a) {}
template<typename T> void prPQ(const priority_queue<T> &fdfsfv_a) {}
*/

// --------------------------------- Quick type ------------------------------------------------------ //
#if defined(QUICK_TYPING)
// #define int long long
#define ll long long
#define ull unsigned long long

// LLONG_MAX, LLONG_MIN
const long long INF_MAX = 9187201950435737471;      // 9.19E18 (0x7f)
const long long INF_MIN = -9114861777597660799;    // -9.11E18 (-0x7f)
const double EPS = 1e-9;
#define PI (2*acos((double)0))

typedef pair<int,int> pii;
typedef vector<pair<int,int>> vii;
#define fi first
#define se second
typedef vector<int> vi;
typedef vector<vector<int>> vvi;
typedef vector<string> vs;
template<typename T> inline bool DEC(T fdfsfv_i,T fdfsfv_j) {return((fdfsfv_i)>(fdfsfv_j));}

#define pb push_back
#define len(a) (int)(a).size()
#define all(a) (a).begin(), (a).end()
#define ms(a,x) memset((a),(x),sizeof(a))
    //Care: Init with 0,-1,0x7f,-0x7f,char[] and bool(true,false)

#define REP(a) for(int idx=0,_a=(a);idx<_a;++idx)
#define FOR(idx,start,end) for(int (idx)=(start),_end=(end);(idx)<_end;++(idx)) // [start, end)
#define FORD(idx,start,end) for(int (idx)=(start),_end=(end);(idx)>_end;--(idx)) // [start, end)
#define FORIT(itx, A) for(auto(itx) = ((A).begin()), _end((A).end()); (itx) != _end; ++(itx))
#endif

// ------------------------------------------ DEBUG ----------------------------------------------------- //
#ifndef FOR(idx,start,end)
    #define FOR(idx,start,end) for(int (idx)=(start),_end=(end);(idx)<_end;++(idx)) // [start, end)
#endif
#ifndef FORIT(itx, A)
    #define FORIT(itx, A) for(auto(itx) = ((A).begin()), _end((A).end()); (itx) != _end; ++(itx)) 
#endif
#ifndef quit
    #define quit exit(0);
#endif
#ifndef nl
    #define nl cerr<<endl;
#endif

template <typename T, typename TT>
inline ostream &operator<<(ostream &fdfsfv_s, pair<T, TT> fdfsfv_t) { return fdfsfv_s << "(" << fdfsfv_t.first << "," << fdfsfv_t.second << ")"; }
#define pr(args...)                                         \
    {                                                       \
        vector<string> fdfsfv_v = fdfsfv_split(#args, ','); \
        fdfsfv_err(fdfsfv_v.begin(), args);                 \
        cerr << endl;                                       \
    }
vector<string> fdfsfv_split(const string &fdfsfv_str, char fdfsfv_c)
{
    vector<string> fdfsfv_vstr;
    stringstream ssss_(fdfsfv_str);
    string fdfsfv_x;
    while (getline(ssss_, fdfsfv_x, (fdfsfv_c)))
    {
        fdfsfv_vstr.emplace_back(fdfsfv_x);
    }
    return move(fdfsfv_vstr);
}
void fdfsfv_err(vector<string>::iterator fdfsfv_itx) {}
template <typename T, typename... Args>
void fdfsfv_err(vector<string>::iterator fdfsfv_itx, T fdfsfv_a, Args... args)
{
    cerr << '[' << (fdfsfv_itx)->substr((*(fdfsfv_itx))[0] == ' ', (fdfsfv_itx)->length()) << "]:" << (fdfsfv_a) << " ";
    fdfsfv_err(++(fdfsfv_itx), (args)...);
}
#define pra(A, start, end)                                                                     \
    {                                                                                          \
        cerr << #A << '[' << (start) << ',' << ((end)-1) << "] = ";                            \
        FOR(fdfsfv_, (start), (end)) { cerr << '[' << fdfsfv_ << ']' << (A)[fdfsfv_] << ' '; } \
        cerr << endl;                                                                          \
    }
#define pri(A)                                                     \
    {                                                              \
        int fdfsfv_cnt__ = 0;                                      \
        cerr << #A << " = ";                                       \
        FORIT(fdfsfv_it__, (A)) { cerr << '[' << fdfsfv_cnt__++ << ']' << (*fdfsfv_it__) << ' '; } \
        cerr << endl;                                              \
    }
#define pri_(A)                                                     \
    {                                                              \
        cerr << #A << " = ";                                       \
        FORIT(fdfsfv_it__, (A)) { cerr << (*fdfsfv_it__) << ' '; } \
        cerr << endl;                                              \
    }
#define prkv(A)                                                    \
    {                                                              \
        cerr << #A << " = ";                                       \
        FORIT(fdfsfv_it__, (A)) { cerr << '[' << (fdfsfv_it__->first) << ']' << (fdfsfv_it__->second) << ' '; } \
        cerr << endl;                                              \
    }
#define praa(A, start1, end1, start2, end2)                                                        \
    {                                                                                              \
        cerr << "----- " << #A << " -----" << endl;                                                \
        FOR(fdfsfv_, (start1), (end1))                                                             \
        {                                                                                          \
            FOR(fdfsfv__, (start2), (end2))                                                        \
            {                                                                                      \
                ostringstream fdfsfv_ssx;                                                          \
                fdfsfv_ssx << '[' << fdfsfv_ << "][" << fdfsfv__ << "]" << (A)[fdfsfv_][fdfsfv__]; \
                cerr << setw(17) << left << fdfsfv_ssx.str();                                      \
            }                                                                                      \
            cerr << endl;                                                                          \
        }                                                                                          \
    }
#define prii(A)                                                    \
    {                                                              \
        int fdfsfv_i__ = 0, fdfsfv_j__ = 0;                        \
        cerr << "----- " << #A << " -----" << endl;                \
        FORIT(fdfsfv_it_i_, (A)) {                                 \
            fdfsfv_j__ = 0;                                        \
            FORIT(fdfsfv_it_j_, (*fdfsfv_it_i_)) {                 \
                ostringstream fdfsfv_ssx;                          \
                fdfsfv_ssx << '[' << fdfsfv_i__ << "][" << fdfsfv_j__++ << "]" << (*fdfsfv_it_j_) << ' '; \
                cerr << setw(17) << left << fdfsfv_ssx.str();      \
            }                                                      \
            fdfsfv_i__ += 1; cerr << endl;                         \
        }                                                          \
        cerr << endl;                                              \
    }


template <typename T>
void prS(const stack<T> &fdfsfv_a)
{
    stack<T> fdfsfv_S(fdfsfv_a);
    cerr << "stack =  [top] ";
    while (!fdfsfv_S.empty())
    {
        cerr << fdfsfv_S.top() << ' ';
        fdfsfv_S.pop();
    }
    cerr << endl;
}
template <typename T>
void prQ(const queue<T> &fdfsfv_a)
{
    queue<T> fdfsfv_Q(fdfsfv_a);
    cerr << "queue =  [front] ";
    while (!fdfsfv_Q.empty())
    {
        cerr << fdfsfv_Q.front() << ' ';
        fdfsfv_Q.pop();
    }
    cerr << "[back]" << endl;
}
template <typename T>
void prPQ(const priority_queue<T> &fdfsfv_a)
{
    priority_queue<T> fdfsfv_PQ(fdfsfv_a);
    cerr << "heap =  [top] ";
    while (!fdfsfv_PQ.empty())
    {
        cerr << fdfsfv_PQ.top() << ' ';
        fdfsfv_PQ.pop();
    }
    cerr << endl;
}
