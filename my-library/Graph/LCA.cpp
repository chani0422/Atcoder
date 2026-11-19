
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから


/* From chanini library */
// LCA（最小共通祖先）: ダブリング
// 前処理 O(N log N)、クエリ O(log N)
// 0-indexed、連結な無向木を想定
//
// 【基本】
// LCA lca(G);                 // 根を0として前処理
// LCA lca(G, root);           // 根を指定
// lca.query(u, v);            // 最小共通祖先
// lca.dist(u, v);             // 距離（辺の本数）
// lca.weighted_dist(u, v);    // 距離（重みの総和）
// lca.calc(u, v);             // {距離, LCA}
//
// 【祖先・パス】
// lca.kth_ancestor(v, k);     // vのk個上
// lca.jump(u, v, k);          // uからvへk辺進んだ頂点
// lca.is_ancestor(a, b);      // aはbの祖先か
// lca.on_path(x, u, v);       // xはu-vパス上か
// lca.child_on_path(a, b);    // 祖先aからbへ進む最初の子
// lca.parent(v);              // vの親（根なら-1）
//
// 【根の変更】どちらもO(logN)
// lca.reroot_lca(u, v, r);    // 根をrに変更したLCA
// lca.subtree_size(v, r);     // 根をrに変更した部分木サイズ
//
// 【配列】
// lca.depth[v];               // 深さ
// lca.sz[v];                  // 部分木サイズ
// lca.tin[v], lca.tout[v];    // 部分木区間 [tin,tout)
// lca.order[i];               // DFS順のi番目の頂点
// lca.root_dist[v];           // 根からvまでの重み付き距離
// lca.up[k][v];               // vの2^k個上の祖先

struct LCA {
    int n, LOG, root;
    vector<vector<int>> up;
    vector<int> depth, tin, tout, sz, order;
    vector<ll> root_dist;

    // コンストラクタ
    // 重みなし: Graph
    // 重み付き: vector<vector<pair<ll,ll>>>
    // 重み付きの辺は {行き先, 重み}
    template<class Edge>
    LCA(const vector<vector<Edge>>& G, int r = 0) {
        build(G, r);
    }

    // 前処理
    template<class Edge>
    void build(const vector<vector<Edge>>& G, int r = 0) {
        n = (int)G.size();
        assert(n > 0 && 0 <= r && r < n);

        root = r;
        LOG = 1;
        while ((1LL << LOG) <= n) LOG++;

        up.assign(LOG, vector<int>(n, -1));
        depth.assign(n, -1);
        tin.assign(n, -1);
        tout.assign(n, -1);
        sz.assign(n, 0);
        root_dist.assign(n, 0);
        order.clear();
        order.reserve(n);

        // 非再帰DFSで親、深さ、部分木情報を構築
        vector<int> idx(n, 0), st;
        st.push_back(root);
        up[0][root] = root;
        depth[root] = 0;

        int timer = 0;

        while (!st.empty()) {
            int v = st.back();

            // 頂点に初めて入ったとき
            if (idx[v] == 0) {
                tin[v] = timer++;
                order.push_back(v);
                sz[v] = 1;
            }

            // 子の探索が終了したとき
            if (idx[v] == (int)G[v].size()) {
                tout[v] = timer;
                st.pop_back();

                if (v != root) {
                    sz[up[0][v]] += sz[v];
                }
                continue;
            }

            const auto& e = G[v][idx[v]++];

            int to;
            ll w;

            if constexpr (is_integral_v<Edge>) {
                // 重みなし
                to = (int)e;
                w = 1;
            } else {
                // 重み付き {行き先, 重み}
                to = (int)e.first;
                w = (ll)e.second;
            }

            if (depth[to] != -1) continue;

            up[0][to] = v;
            depth[to] = depth[v] + 1;
            root_dist[to] = root_dist[v] + w;
            st.push_back(to);
        }

        assert(timer == n);

        // ダブリング表の構築
        for (int k = 1; k < LOG; k++) {
            for (int v = 0; v < n; v++) {
                up[k][v] = up[k-1][up[k-1][v]];
            }
        }
    }

    // vのk個上の祖先（存在しなければ-1）
    int kth_ancestor(int v, ll k) const {
        if (k < 0 || k > depth[v]) return -1;

        for (int b = 0; b < LOG; b++) {
            if ((k >> b) & 1LL) {
                v = up[b][v];
            }
        }
        return v;
    }

    // vの親（根なら-1）
    int parent(int v) const {
        return (v == root) ? -1 : up[0][v];
    }

    // u,vの最小共通祖先
    int query(int u, int v) const {
        if (depth[u] < depth[v]) swap(u, v);

        // 深さを揃える
        u = kth_ancestor(u, depth[u] - depth[v]);

        if (u == v) return u;

        // 大きいジャンプから順に登る
        for (int b = LOG - 1; b >= 0; b--) {
            if (up[b][u] != up[b][v]) {
                u = up[b][u];
                v = up[b][v];
            }
        }

        return up[0][u];
    }

    // u,v間の距離（辺の本数）
    int dist(int u, int v) const {
        int p = query(u, v);
        return depth[u] + depth[v] - 2 * depth[p];
    }

    // u,v間の距離（辺の重みの総和）
    ll weighted_dist(int u, int v) const {
        int p = query(u, v);
        return root_dist[u] + root_dist[v] - 2 * root_dist[p];
    }

    // {辺数としての距離, LCA} を返す
    pair<int,int> calc(int u, int v) const {
        int p = query(u, v);
        int d = depth[u] + depth[v] - 2 * depth[p];
        return {d, p};
    }

    // uからvへk辺進んだ頂点
    // 0ならu、dist(u,v)ならv
    // パスの外なら-1
    int jump(int u, int v, int k) const {
        int p = query(u, v);
        int a = depth[u] - depth[p];
        int d = a + depth[v] - depth[p];

        if (k < 0 || k > d) return -1;

        if (k <= a) {
            return kth_ancestor(u, k);
        } else {
            return kth_ancestor(v, d - k);
        }
    }

    // aはbの祖先か（a==bもtrue）
    bool is_ancestor(int a, int b) const {
        return tin[a] <= tin[b] && tin[b] < tout[a];
    }

    // xはu-vパス上にあるか
    bool on_path(int x, int u, int v) const {
        return dist(u, x) + dist(x, v) == dist(u, v);
    }

    // aがbの祖先である場合、aからbへ進む最初の子
    // 条件を満たさなければ-1
    int child_on_path(int a, int b) const {
        if (a == b || !is_ancestor(a, b)) return -1;
        return kth_ancestor(b, depth[b] - depth[a] - 1);
    }

    // 根をrに変更した場合のLCA
    // 前処理のやり直しは不要
    int reroot_lca(int u, int v, int r) const {
        int a = query(u, v);
        int b = query(u, r);
        int c = query(v, r);

        if (depth[a] < depth[b]) a = b;
        if (depth[a] < depth[c]) a = c;

        return a;
    }

    // 根をrに変更した場合のvの部分木サイズ
    int subtree_size(int v, int r) const {
        if (v == r) return n;

        if (!is_ancestor(v, r)) {
            return sz[v];
        }

        int child = child_on_path(v, r);
        return n - sz[child];
    }
};


//問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    
    return 0;
}
//解答コードここまで

#else

// === LIB_BEGIN ===
#include <bits/stdc++.h>
#include <atcoder/all>
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
using Grid = vector<vector<char>>;


//====== ここから入出力系 ======
template<class A, class B>
ostream& operator<<(ostream& os, const pair<A,B>& p) {
    return os << "(" << p.first << "," << p.second << ")";
}

//引数を空白区切りで出力
template<class T, class... Ts>
void out(const T& a, const Ts&... b){
    cout << a;
    ((cout << ' ' << b), ...);
    cout << '\n';
}
//引数を1行づつ出力
template<class T, class... Ts>
void outendl(const T& a, const Ts&... b){
    cout << a;
    ((cout << endl << b), ...); 
    cout << '\n';
}
//引数を順に読み込む
template <class... T>
void input(T &...a) {
	(cin >> ... >> a);
}

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

// === LIB_END ===

#endif