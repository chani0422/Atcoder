
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
//E問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n; cin >> n;
    Graph G(n);
    vector<ll> indeg(n,0);
    vector<ll> leaf(n,0);
    rep(i,0,n-1) {
        ll a,b; cin >> a >> b;
        a--;b--;
        indeg[a]++;
        indeg[b]++;
        G[a].pb(b);
        G[b].pb(a);
    }
    vector<ll> C(n+1);
    rep(i,0,n) cin >> C[i];

    queue<ll> que;
    ll center = -1;
    rep(i,0,n) if(indeg[i] == 1) {
        que.push(i);
        center = i;
        leaf[i] = 1;
    }
    while(!que.empty()) {
        ll from = que.front();
        que.pop();
        center = from;
        for(auto to : G[from]) {
            indeg[to]--;
            if(indeg[to] == 1) que.push(to);
        }
    }

    //類題
    //https://atcoder.jp/contests/abc361/submissions/80383665
    vector<ll> dist(n,-1);
    dist[center] = 0;

    auto dfs = [&](auto dfs,ll from)->void {
        for(auto to : G[from]) {
            if(dist[to] != -1) continue;
            dist[to] = dist[from] + 1;
            dfs(dfs,to);
        }
    };

    ll first = -1;
    dfs(dfs,center);
    first = max_element(nall(dist))-dist.begin();
    vector<ll> ndist = dist;
    // dist.assign(n,-1);
    // dist[first] = 0;
    // dfs(dfs,first);
    ll mx = *max_element(nall(ndist));
    // cout << center << " " << T << "\n";

    map<ll,ll> count;
    rep(i,0,n) {
        if(leaf[i] == 1 && ndist[i] == mx) {
            count[C[i]]++;
        }
    }

    
    //葉である頂点uから見た時、他に直径になりうるvをカウントしたい。
    ll ans = 0;
    //奇数
    if(n%2 == 0) {
        for(auto [col,cnt] : count) {
            ans += cnt*(cnt-1)/2;
        }
    }else {
        queue<pair<ll,ll>> que1;
        vector<pair<ll,ll>> sz(n,{-1,-1});
        ll id = 0;
        for(auto to : G[center]) {
            que1.emplace(to,id);
            sz[to] = {0,id};
            id++;
        }

        while(!que1.empty()) {
            auto [from, col] = que1.front();
            que1.pop();
            for(auto to : G[from]) {
                auto [d, col] = sz[to];
                if(d == -1) {
                    sz[to] = {d + 1, col};
                    que1.emplace(to,col);
                }
            }
        }

        map<ll,ll> visited;
        auto [MAX,col] = *max_element(nall(sz));
        set<ll> can;
        rep(i,0,n) {
            if(leaf[i] == 1 && sz[i].fi == MAX && visited[col] == 0) {
                can.insert(col);
                visited[col] = 1;
            }
        }
        map<ll,ll> visited1;
        for(auto to : G[center]) {
            if(can.contains)
        }
    }

    cout << ans << "\n";
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
