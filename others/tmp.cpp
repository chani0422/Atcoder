
#ifdef INCLUDED_MAIN

//解答コードここから
//D問題
signed main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ll h,w; cin >> h >> w;
    Graphchar g(h+2,vector<char>(w+2,'#'));
    Graphll dist(h+2,vector<ll>(w+2,-1));
    map<char,bool> used;
    map<char,vector<pair<ll,ll>>> mp;
    int di[4] = {1,-1,0,0};
    int dj[4] = {0,0,1,-1};
    rep(i,1,h+1) {
        rep(j,1,w+1) {
            char c; cin >> c;
            g[i][j] = c;
            pair<ll,ll> p = {i,j};
            if('a' <= c && c <= 'z') mp[c].pb(p);
        }
    }
    ll ans = 0;
    queue<pair<ll,ll>> q;
    q.push({1,1});
    dist[1][1] = 0;



    while(!q.empty()) {
        pair<ll,ll> v = q.front();
        //cout << v.fi << " " << v.se << endl;
        q.pop();
        char c = g[v.fi][v.se];
        t6
        //if(g[v.fi][v.se] == '#') continue;
        //cout << "char: " << c << endl;
        if('a' <= g[v.fi][v.se] && g[v.fi][v.se] <= 'z'){
            if(used[c] == false) {
                used[c] = true;
                for(auto p : mp[c]) {
                    pair<ll,ll> v2 = {p.fi,p.se};
                    if(dist[p.fi][p.se] == -1) {
                        dist[p.fi][p.se] = dist[v.fi][v.se] + 1;
                        q.push(v2);
                    }
                }
            }
        }
        for(int iii = 0; iii < 4; iii++) {
            ll ni = v.fi + di[iii], nj = v.se + dj[iii];
            //cout << g[ni][nj] << dist[ni][nj] << endl;
            if(g[ni][nj] == '#' || dist[ni][nj] != -1) continue;
            //cout << "next: " << ni << " " << nj << endl;
            dist[ni][nj] = dist[v.fi][v.se] + 1;
            pair<ll,ll> p2 = {ni,nj};
            q.push(p2);
    }
//     cout << q.size() << endl;
//         rep(i,0,h+2) {
//     rep(j,0,w+2) {
//         cout << dist[i][j];
//     }
//     cout << endl;
// }
    }

    if(dist[h][w] == -1) cout << -1 << endl;
    else cout << dist[h][w] << endl;
    return 0;
}
 //解答コードここまで

#else
// === LIB_BEGIN ===
#include <bits/stdc++.h>
using namespace std;

#define rep(i,a,b) for(int i=(a); i<(b); i++)
#define rrep(i,a,b) for(int i=(a); i>=(b); i--)
#define pb(x) push_back(x)
#define popb() pop_back()
#define fi first
#define se second
#define yn(x) do { if(x) cout << "Yes\n"; else cout << "No\n"; } while(0)
#define nall(a) a.begin(),a.end()
#define rall(a) a.rbegin(),a.rend()
#define For(x) for(auto a : (x)) cout << a << " ";

using ll = long long;
using ld = long double;
using ull = unsigned long long;
using Graph = vector<vector<ll>>;

template<class A, class B>
ostream& operator<<(ostream& os, const pair<A,B>& p) {
    return os << "(" << p.first << "," << p.second << ")";
}

template<class T>
void For2(const T& a) {
    for (const auto& row : a) {
        for (const auto& v : row) cout << v << " ";
        cout << "\n";
    }
}
#define INCLUDED_MAIN
#include __FILE__

// === LIB_END ===
#endif

