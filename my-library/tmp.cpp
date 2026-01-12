#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=a;i<b;i++)
#define rrep(i,a,b) for(int i=a;i>=b;i--)
#define pb(x) push_back(x)
#define popb() pop_back()
#define fi first
#define se second
#define yn(x) do { if(x) cout << "Yes\n"; else cout << "No\n"; } while(0)
#define nall(a) a.begin(),a.end()
#define rall(a) a.rbegin(),a.rend()
#define For(x) for(auto a : x)cout << a << " ";
using ll = long long; using ld = long double;
using ull = unsigned long long;
using Graph = vector<vector<ll>>;

// ===================== BFS / DFS templates =====================

// BFS（辺重み 1 の最短距離）
// dist=-1: 未到達, parent=-1: 親なし
pair<vector<ll>, vector<ll>> bfs(const Graph& G, ll s) {
    ll n = (ll)G.size();
    vector<ll> dist(n,-1), parent(n,-1);
    queue<ll> q;

    dist[s] = 0;
    q.push(s);

    while(!q.empty()) {
        ll v = q.front(); q.pop();
        for(ll to : G[v]) {
            if(dist[to] != -1) continue;
            dist[to] = dist[v] + 1;
            parent[to] = v;
            q.push(to);
        }
    }
    return {dist, parent};
}

// parent から s->t の経路復元（到達不可なら空）
vector<ll> restore_path(ll s, ll t, const vector<ll>& parent) {
    vector<ll> path;
    for(ll cur = t; cur != -1; cur = parent[cur]) path.pb(cur);
    reverse(nall(path));
    if(path.empty() || path[0] != s) return {};
    return path;
}

// DFS（再帰）
void dfs_rec(ll v, const Graph& G, vector<char>& used) {
    used[v] = 1;
    for(ll to : G[v]) {
        if(used[to]) continue;
        dfs_rec(to, G, used);
    }
}

// DFS（非再帰：スタック）
void dfs_iter(ll s, const Graph& G, vector<char>& used) {
    stack<ll> st;
    st.push(s);
    while(!st.empty()) {
        ll v = st.top(); st.pop();
        if(used[v]) continue;
        used[v] = 1;
        for(ll to : G[v]) {
            if(!used[to]) st.push(to);
        }
    }
}

// ===============================================================

//◯問題
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);

    // 使い方例（必要なときだけコメント外して使って）
    /*
    ll n, m; cin >> n >> m;
    Graph G(n);
    rep(i,0,m){
        ll a,b; cin >> a >> b;
        --a; --b;
        G[a].pb(b);
        G[b].pb(a); // 無向なら
    }

    // BFS
    auto [dist, parent] = bfs(G, 0);

    // DFS
    vector<char> used(n, 0);
    dfs_rec(0, G, used);      // or dfs_iter(0, G, used);
    */

    return 0;
}
