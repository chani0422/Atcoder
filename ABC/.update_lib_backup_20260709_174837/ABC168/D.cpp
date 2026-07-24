
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//D問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m; cin >> n >> m;          // 頂点数n, 辺数m

    Graph G(n);
    rep(i,0,m){
        int u, v; cin >> u >> v;
        --u; --v;
        G[u].push_back(v);
        G[v].push_back(u);          // 無向グラフなら両方（有向なら片方だけ）
    }
    vector<int> dist(n, -1);
    vector<int> parent(n, -1);        // 経路復元用（親頂点）
    queue<int> q;

    dist[0] = 0;
    parent[0] = -2;                   // 始点マーカー（別に無くてもOK）
    q.push(0);

    while(!q.empty()){
        int v = q.front();
        q.pop();

        for(int nv : G[v]){
            if(dist[nv] != -1) continue; // 既訪問

            dist[nv] = dist[v] + 1;
            parent[nv] = v;
            q.push(nv);
        }
    }

    // 到達できなかったら -1
    if(dist[n-1] == -1) {
        cout << "No\n";
        return 0;
    }
    cout << "Yes\n";
    rep(i,1,parent.size()) {
        cout << parent[i] + 1 << "\n";
    }

    //From chanini library
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
using Grid = vector<vector<ll>>;


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

