// //Library
#include<bits/stdc++.h>
using namespace std;
const long long inf = (1LL<<60);

#define rep(i,a,b) for(ll i=(a); i<(b); i++)
#define rrep(i,a,b) for(ll i=(a); i>=(b); i--)
#define pb push_back
#define popb() pop_back()
#define fi first
#define se second
#define yn(x) do { if(x) cout << "Yes\n"; else cout << "No\n"; } while(0)
#define nall(a) (a).begin(),(a).end()
#define rall(a) (a).rbegin(),(a).rend()
#define MIN(x) *min_element(nall(x))
#define MAX(x) *max_element(nall(x))

using ll = long long;
using ld = long double;
using ull = unsigned long long;
using lll = __int128_t;

template<class T> using vc   = vector<T>;
template<class T> using vvc  = vector<vc<T>>;
template<class T> using vvvc = vector<vvc<T>>;

using Graph= vector<vector<ll>>;
using WGraph = vector<vector<pair<ll,ll>>>;
using Grid = vector<vector<char>>;

//上下左右移動用
const int dx[4] = {1, 0, -1, 0};
const int dy[4] = {0, 1, 0, -1};
const int ddx[8] = {1, 1, 1, 0, 0, -1, -1, -1};
const int ddy[8] = {1, 0, -1, 1, -1, 1, 0, -1};


// __int128_tでも入出力できるようにする
// cout 用
ostream& operator<<(ostream& os, lll x) {
    if (x == 0) return os << '0';
    if (x < 0) { os << '-'; x = -x; } // 通常ケース

    string s;
    while (x > 0) {
        int d = (int)(x % 10);
        s.push_back(char('0' + d));
        x /= 10;
    }
    reverse(s.begin(), s.end());
    return os << s;
}

// cin 用
istream& operator>>(istream& is, lll& x) {
    string s;
    is >> s;
    bool neg = false;
    int i = 0;
    if (!s.empty() && s[0] == '-') { neg = true; i = 1; }

    lll val = 0;
    for (; i < (int)s.size(); i++) {
        val = val * 10 + (s[i] - '0');
    }
    x = neg ? -val : val;
    return is;
}

//===== ここから便利系 =====
//ans = min(ans,x)をやる。ついでに戻り値もtrue,falseで返す。
template <class T>
bool chmin(T& a, const T& b) {
    if (b < a) { a = b; return true; }
    return false;
}
//ans = max(ans,x)をやる。ついでに戻り値もtrue,falseで返す。
template <class T>
bool chmax(T& a, const T& b) {
    if (a < b) { a = b; return true; }
    return false;
}

//===== ここからデバッグ系 =====
template<class Array2D>
void For2(const Array2D& a) {
    for (const auto& row : a) {
        for (const auto& v : row) cout << v << " ";
        cout << "\n";
    }
}
template <class Array1D>
void For(const Array1D& x) {
    for (const auto& a : x) cout << a << " ";
    cout << '\n';
}

int main() {
    while(1) {
    ll n,m,t,p; cin >> n >> m >> t >> p;
    if(n == 0 && m == 0 && t == 0 && p == 0) break;
    vector<vector<ll>> G(m,vector<ll>(n,1));
    ll under = m-1, left = 0;
    rep(i,0,t) {
        ll d,c; cin >> d >> c;
        if(d == 1) {
            left += c;
            rep(k,0,under) {
                rep(j,left,n) {
                    if(k+c == m) break;
                    G[k][j] += G[k+c][j];
                }
            }
        }else if(d == 2) {
            under -= c;
            rep(k,under,under+c) {
                rep(j,left,n) {
                    if(k+c == m) break;
                    G[k][j] += G[k+c][j];
                }
            }
        }
    }

    ll ans = 0;
    rep(i,0,p) {
        ll x,y; cin >> x >> y;
        ans += G[y][x];
    }

    cout << ans << endl;
}
    return 0;
}


// 2 1 1 1
// 1 1
// 0 0
// 0 0 0 0 

// //ダイクストラ法 ※Graphの型を確認
// int main(){
//     ll n,m; cin >> n >> m;
//     WGraph G(n);
//     vector<ll> dist(n,inf);
//     rep(i,0,m) {
//         ll u,v,c; cin >> u >> v >> c;
//         u--;v--;
//         G[u].emplace_back(v,c);
//     }

//     priority_queue<pair<ll,ll>,vector<pair<ll,ll>>,greater<pair<ll,ll>>> pq;
//     ll start_v = 0;
//     pq.push({0,start_v});
//     dist[start_v] = 0;

//     while(!pq.empty()) {
//         auto [d,v] = pq.top();
//         pq.pop();
//         if(d != dist[v]) continue;

//         for(auto [to,c] : G[v]) {
//             if(dist[to] > d + c) {
//             dist[to] = d + c;
//             pq.push({dist[to],to});
//             }
//         }
//     }

//     //出力
//     rep(i,0,n) {
//         if(dist[i] == inf) {
//             cout << -1 << endl;
//         }else {
//             cout << dist[i] << "\n";
//         }
//     }
//     return 0;
// }

// // DFS隣接リスト
// vector<bool> visited;
// Graph G;

// void dfs(int v) {
//     //この関数を使うときはグローバルで配列を作る。
//     visited[v] = true;
//     for(auto &to : G[v]) {
//         if(visited[to] == true) continue;
//         dfs(to);
//     }
// }
// //再帰とstack使い分ける
// int main() {
//     ios::sync_with_stdio(false);cin.tie(nullptr);
//     ll n,m; cin >> n >> m;
//     G.resize(n);
//     visited.resize(n,false);
//     vector<ll> A(m),B(m);
//     rep(i,0,m) {
//         cin >> A[i] >> B[i];
//         A[i]--;B[i]--;
//         G[A[i]].push_back(B[i]);
//         G[B[i]].push_back(A[i]);
//     }

//     stack<int> st;
//     st.push(0);
//     visited[0] = true;

//     while(!st.empty()) {
//         int v = st.top();
//         st.pop();
//         for(auto to : G[v]) {
//             if(visited[to] == true) continue;
//             visited[to] = true;
//             st.push(to);
//         }
//     }
//     // dfs(0);
//     rep(i,0,n) {
//         if(visited[i] == false) {
//             cout << i+1 << endl;
//         }
//     }
//     return 0;
// }

// // DFSグリッド
// int h, w;
// vector<string> grid;
// vector<vector<bool>> seen;
// vector<vector<pair<int,int>>> parent;

// void dfs(ll x, ll y) {
//     seen[x][y] = true;

//     rep(i,0,4) {
//         ll nx = x + dx[i];
//         ll ny = y + dy[i];

//         if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue;
//         if(grid[nx][ny] == '#') continue;
//         if(seen[nx][ny]) continue;

//         parent[nx][ny] = {x,y};
//         dfs(nx,ny);
//     }
// }

// int main() {
//     ios::sync_with_stdio(false); cin.tie(nullptr);

//     cin >> h >> w;
//     grid.resize(h);
//     seen.assign(h,vector<bool>(w,false));
//     parent.assign(h, vector<pair<int,int>>(w, {-1, -1}));

//     int sx, sy, gx, gy; cin >> sx >> sy >> gx >> gy;
//     --sx; --sy; --gx; --gy;

//     rep(i,0,h) cin >> grid[i];
//     // rep(i,0,h) {
//     //     rep(j,0,w) {
//     //         if(grid[i][j] == 'S') {
//     //             sx = i;
//     //             sy = j;
//     //         }else if(grid[i][j] == 'G') {
//     //             gx = i;
//     //             gy = j;
//     //         }
//     //     }
//     // }

//     dfs(sx,sy);

//     //経路復元
//     vector<pair<int,int>> path;

//     if(seen[gx][gy]) {
//         ll x = gx;
//         ll y = gy;

//         while(!(x == sx && y == sy)) {
//             path.push_back({x,y});

//             auto[px, py] = parent[x][y];
//             x = px;
//             y = py;
//         }

//         path.push_back({sx,sy});

//         reverse(nall(path));
//     }

    

//     if(seen[gx][gy]) cout << "Yes" << endl;
//     else cout << "No" << endl;

//     return 0;
// }

// // BFS隣接リスト
// int main() {
//     ios::sync_with_stdio(false);
//     cin.tie(nullptr);

//     int n, m; cin >> n >> m;
//     int s, g; cin >> s >> g;
//     --s; --g;

//     Graph G(n);
//     rep(i,0,m){
//         int u, v; cin >> u >> v;
//         --u; --v;
//         G[u].push_back(v);
//         G[v].push_back(u);
//         }

//     vector<int> dist(n, -1);
//     vector<int> parent(n, -1);
//     queue<int> q;

//     dist[s] = 0;
//     parent[s] = -2;
//     q.push(s);

//     while(!q.empty()){
//         int v = q.front();
//         q.pop();

//         for(int nv : G[v]){
//             if(dist[nv] != -1) continue; // 既訪問

//             dist[nv] = dist[v] + 1;
//             parent[nv] = v;
//             q.push(nv);
//         }
//     }

//     // 到達できなかったら -1
//     cout << dist[g] << endl;

//     // ======= 経路復元したいとき =======
//     /*
//     if(dist[g] != -1){
//         vector<int> path;
//         int cur = g;
//         while(cur != s){
//             path.push_back(cur);
//             cur = parent[cur];
//         }
//         path.push_back(s);

//         //1-indexに戻して出力
//         reverse(nall(path));
//         for(int v : path){
//             cout << v+1 << "\n";
//         }
//     }
//     */

//     //From chanini library
//     return 0;
// }

// // BFSグリッド
// int main() {
//     ios::sync_with_stdio(false); cin.tie(nullptr);

//     int h, w; cin >> h >> w;
//     int sx, sy, gx, gy; cin >> sx >> sy >> gx >> gy;
//     --sx; --sy; --gx; --gy;

//     vector<string> g(h);
//     rep(i,0,h) cin >> g[i];
//     // rep(i,0,h) {
//     //     rep(j,0,w) {
//     //         if(g[i][j] == 'S') {
//     //             sx = i;
//     //             sy = j;
//     //         }else if(g[i][j] == 'G') {
//     //             gx = i;
//     //             gy = j;
//     //         }
//     //     }
//     // }

//     //上下左右移動用
//     const int dx[4] = {1, 0, -1, 0};
//     const int dy[4] = {0, 1, 0, -1};

//     vector<vector<ll>> dist(h, vector<ll>(w, -1));
//     vector<vector<pair<int,int>>> parent(h, vector<pair<int,int>>(w, {-1,-1}));
//     queue<pair<int,int>> q;

//     dist[sx][sy] = 0;
//     parent[sx][sy] = {-2,-2}; // 始点マーカー（別に無くてもOK）
//     q.push({sx, sy});

//     while(!q.empty()){
//         auto [x, y] = q.front();
//         q.pop();

//         rep(dir,0,4){
//             int nx = x + dx[dir];
//             int ny = y + dy[dir];

//             if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue; // 範囲外
//             if(g[nx][ny] == '#') continue; // 壁
//             if(dist[nx][ny] != -1) continue; // 既訪問

//             dist[nx][ny] = dist[x][y] + 1;
//             parent[nx][ny] = {x, y};
//             q.push({nx, ny});
//         }
//     }
//     //到達できなかったら-1を出力
//     cout << dist[gx][gy] << endl;

//     // ======= 経路復元したいとき=======
//     /*
//     if(dist[gx][gy] != -1){
//         vector<pair<int,int>> path;
//         pair<int,int> cur = {gx, gy};
//         while(!(cur.fi == sx && cur.se == sy)){
//             path.pb(cur);
//             cur = parent[cur.fi][cur.se];
//         }
//         path.pb({sx, sy});
//         reverse(nall(path));

//         // 例：経路の座標を出力（1-indexに戻す）
//         for(auto [x,y] : path){
//             cout << x+1 << " " << y+1 << "\n";
//         }
//     }
//     */
//     return 0;
// }

// // Bellman-Ford
// int main() {
//     ios::sync_with_stdio(false);cin.tie(nullptr);
//     ll n,m; cin >> n >> m;

//     vector<ll> A(m),B(m),C(m);
//     vector<ll> dist(n,inf);

//     rep(i,0,m) {
//         ll a,b,c; cin >> a >> b >> c;
//         a--;b--;
//         A[i] = a;
//         B[i] = b;
//         C[i] = c;
//     }
//     cout << "\n";
//     For(A);
//     For(B);
//     For(C);

//     dist[0] = 0;

//     rep(i,0,n-1) {
//         bool update = false;
//         rep(j,0,m) {
//             ll from = A[j],to = B[j],cost = C[j];
//             if(dist[from] == inf) continue;
//             if(dist[to] > dist[from] + cost) {
//                 dist[to] = dist[from] + cost;
//                 update = true;
//             }
//         }
//         if(!update) break;
//     }

//     bool neg = false;
//     rep(i,0,m) {
//         ll from = A[i],to = B[i],cost = C[i];
//         if(dist[from] == inf) continue;
//         if(dist[to] > dist[from] + cost) neg = true;
//     }

//     if(neg) {
//         cout << "NEGATIVE CYCLE\n";
//         return 0;
//     }

//     rep(i,0,n) {
//         if(dist[i] == inf) cout << "INF\n";
//         else cout << dist[i] << "\n";
//     }
//     return 0;
// }

// // DFS連結成分
// ll components(const Graph& G){
//     int n = (int)G.size();
//     vector<bool> visited(n, false);
//     ll comps = 0;

//     for(int vertex = 0; vertex < n; vertex++){
//         if(visited[vertex]) continue;
//         comps++;

//         stack<int> st;
//         visited[vertex] = true;
//         st.push(vertex);

//         while(!st.empty()){
//             int v = st.top(); st.pop();
//             for(ll to : G[v]){
//                 int u = (int)to;
//                 if(visited[u]) continue;
//                 visited[u] = true;
//                 st.push(u);
//             }
//         }
//     }
//     return comps;
// }
// int main() {
//     ios::sync_with_stdio(false);cin.tie(nullptr);
//     ll n,m; cin >> n >> m;
//     bool ok = false;
//     Graph G(n);
//     vector<bool> visited(n,false);
//     ll ans = 0;
//     vector<ll> A(m),B(m);
//     rep(i,0,m) {
//         ll a,b; cin >> a >> b;
//         a--;b--;
//         G[a].push_back(b);
//         G[b].push_back(a);
//         A[i] = a; B[i] = b;
//     }

//         stack<int> st;
//         st.push(0);
//         visited[0] = true;

//         while(!st.empty()) {
//             int v = st.top();
//             st.pop();
//             for(auto x : G[v]) {
//                 if(visited[x] == true) continue;
//                 st.push(x);
//                 visited[x] = true;
//             }
//         }

//     rep(i,0,n) if(visited[i] == false) ok = true;
    
//     yn(ok);
//     return 0;
// }