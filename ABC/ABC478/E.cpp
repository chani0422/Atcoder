
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
// 有向グラフ用。頂点番号・成分番号はすべて 0-indexed。
// ACL の Tarjan 法を参考にした、再帰を使わない拡張版。
// 辺追加後、最初の問い合わせで自動 build。計算量・メモリ O(N+M)。
// 異なる成分間の辺は、必ず小さい成分番号から大きい成分番号へ向かう。
class SCCGraph {
    int n_;
    std::vector<std::vector<int>> graph_;
    bool built_ = false;
    bool has_cycle_ = false;

    std::vector<int> comp_;
    std::vector<std::vector<int>> groups_, dag_, reverse_dag_;
    std::vector<int> indegree_, outdegree_;
    std::vector<bool> cyclic_;

    void check_vertex(int v) const {
        assert(0 <= v && v < n_);
    }

public:
    explicit SCCGraph(int n = 0) : n_(n) {
        assert(n >= 0);
        graph_.resize(n);
    }

    int num_vertices() const { return n_; }

    // from -> to を追加。自己ループ・多重辺も可。
    void add_edge(int from, int to) {
        check_vertex(from);
        check_vertex(to);
        graph_[from].push_back(to);
        built_ = false;
    }

    // 自動で呼ばれるので、通常は明示的に呼ばなくてもよい。
    // 辺追加がなければ再計算しない。
    void build() {
        if (built_) return;

        comp_.assign(n_, -1);
        std::vector<int> ord(n_, -1), low(n_), next_edge(n_, 0);
        std::vector<int> dfs_stack, active;
        dfs_stack.reserve(n_);
        active.reserve(n_);
        int timer = 0, k = 0;

        for (int root = 0; root < n_; ++root) {
            if (ord[root] != -1) continue;

            ord[root] = low[root] = timer++;
            dfs_stack.push_back(root);
            active.push_back(root);

            while (!dfs_stack.empty()) {
                int v = dfs_stack.back();

                if (next_edge[v] < (int)graph_[v].size()) {
                    int to = graph_[v][next_edge[v]++];
                    if (ord[to] == -1) {
                        ord[to] = low[to] = timer++;
                        dfs_stack.push_back(to);
                        active.push_back(to);
                    } else {
                        low[v] = std::min(low[v], ord[to]);
                    }
                    continue;
                }

                // v の探索終了。再帰DFSの「親へ戻る」に相当。
                dfs_stack.pop_back();
                if (!dfs_stack.empty()) {
                    int parent = dfs_stack.back();
                    low[parent] = std::min(low[parent], low[v]);
                }

                if (low[v] == ord[v]) {
                    while (true) {
                        int u = active.back();
                        active.pop_back();
                        ord[u] = n_;  // 所属確定済み。以後 low を下げない。
                        comp_[u] = k;
                        if (u == v) break;
                    }
                    ++k;
                }
            }
        }

        // Tarjan法の確定順を反転し、トポロジカル順にする。
        for (int& c : comp_) c = k - 1 - c;
        groups_.assign(k, {});
        for (int v = 0; v < n_; ++v) {
            groups_[comp_[v]].push_back(v);
        }

        dag_.assign(k, {});
        reverse_dag_.assign(k, {});
        indegree_.assign(k, 0);
        outdegree_.assign(k, 0);
        cyclic_.assign(k, false);
        has_cycle_ = false;
        std::vector<int> seen(k, -1);

        for (int c = 0; c < k; ++c) {
            cyclic_[c] = groups_[c].size() >= 2;

            for (int v : groups_[c]) {
                for (int to : graph_[v]) {
                    int d = comp_[to];

                    if (c == d) {
                        if (v == to) cyclic_[c] = true;
                        continue;
                    }

                    // 同じ c -> d は1本にまとめる。ソート不要。
                    if (seen[d] == c) continue;
                    seen[d] = c;

                    dag_[c].push_back(d);
                    reverse_dag_[d].push_back(c);
                    ++outdegree_[c];
                    ++indegree_[d];
                }
            }
            if (cyclic_[c]) has_cycle_ = true;
        }

        built_ = true;
    }

    // ACLと同じ戻り値。各成分の頂点一覧をコピーして返す。
    std::vector<std::vector<int>> scc() {
        build();
        return groups_;
    }

    // ACL内部の scc_ids() に相当。
    // {成分数, 各頂点の成分番号} を返す。
    std::pair<int, std::vector<int>> scc_ids() {
        build();
        return {(int)groups_.size(), comp_};
    }

    int count() { build(); return (int)groups_.size(); }
    int id(int v) { check_vertex(v); build(); return comp_[v]; }
    bool same(int u, int v) { return id(u) == id(v); }
    int size(int v) { return (int)group(v).size(); }

    // v と同じ成分の頂点一覧。引数は「頂点番号」。
    const std::vector<int>& group(int v) {
        int c = id(v);
        return groups_[c];
    }

    // 以下の参照は読み取り専用。辺追加後は取得し直すこと。
    const std::vector<int>& ids() {
        build();
        return comp_;
    }

    const std::vector<std::vector<int>>& groups() {
        build();
        return groups_;
    }

    const std::vector<std::vector<int>>& dag() {
        build();
        return dag_;
    }

    const std::vector<std::vector<int>>& reverse_dag() {
        build();
        return reverse_dag_;
    }

    const std::vector<int>& indegree() {
        build();
        return indegree_;
    }

    const std::vector<int>& outdegree() {
        build();
        return outdegree_;
    }

    // 元のグラフの有向閉路を判定。自己ループも閉路に含む。
    bool has_cycle() {
        build();
        return has_cycle_;
    }

    bool in_cycle(int v) {
        int c = id(v);
        return cyclic_[c];
    }

    // ans[v] = v から閉路に到達できるか。
    // 閉路上の頂点も true。
    std::vector<bool> can_reach_cycle() {
        build();
        std::vector<bool> ok = cyclic_;

        for (int c = (int)groups_.size() - 1; c >= 0; --c) {
            for (int to : dag_[c]) {
                if (ok[to]) {
                    ok[c] = true;
                    break;
                }
            }
        }

        std::vector<bool> ans(n_);
        for (int v = 0; v < n_; ++v) {
            ans[v] = ok[comp_[v]];
        }
        return ans;
    }

    // ans[c] = 成分 c の頂点の重み合計。
    // weight が vector<long long> なら、合計も long long。
    template<class T>
    std::vector<T> component_sum(const std::vector<T>& weight) {
        assert((int)weight.size() == n_);
        build();

        std::vector<T> ans(groups_.size(), T{});
        for (int v = 0; v < n_; ++v) {
            ans[comp_[v]] += weight[v];
        }
        return ans;
    }
};
//E問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n, Q; cin >> n >> Q;
    Graph G(n);
    SCCGraph nG(n);
    vector<pair<ll,ll>> p;

    rep(i,0,Q) {
        ll t,u,v; cin >> t >> u >> v;
        u--;v--;
        G[u].pb(v);
        nG.add_edge(u,v);
        if(t == 1) p.pb({u,v});
    }

    auto [k,comp] = nG.scc_ids();
    auto groups = nG.scc();
    for(auto [u,v] : p) {
        if(comp[u] == comp[v]) {
            cout << "No\n";
            return 0;
        }
    }
    cout << "Yes\n";
    ll id = 1;
    vector<ll> A(n,-1);
    for(auto group : groups) {
        for(auto v : group) {
            A[v] = id;
        }
        id++;
    }
    For(A);
    return 0;
}
//解答コードここまで

#else

// === LIB_BEGIN ===
#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;

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
using mint = modint998244353;

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
