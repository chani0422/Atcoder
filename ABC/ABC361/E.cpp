
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//E問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ll n; cin >> n;
    // Source: https://github.com/chani0422/Atcoder/blob/main/my-library/snipet.txt
    //クラスカル法 From chanini library
    vector<tuple<ll,ll,ll>> edges;
    rep(i,0,n-1) {
        ll a,b,c; cin >> a >> b >> c;
        a--;b--;
        edges.pb({c,a,b});
    }
    
    //辺を重みの小さい順にソート
    sort(rall(edges)); //最大全域木！
    
    dsu uf(n);
    Graph G(n);
    //小さい辺から見ていく
    vector<tuple<ll,ll,ll>> can;
    vector<ll> indeg(n,0);

    for(auto &[c,a,b] : edges) {
        //a,bが違うグループだったら閉路にならないからつなげる
        if(!uf.same(a,b)) {
            uf.merge(a,b);
            G[a].pb(b);
            G[b].pb(a);
            indeg[a]++;
            indeg[b]++;
            can.pb({c,a,b});
        }
    }

    queue<ll> que;
    vector<ll> visited(n,false);
    rep(i,0,n) if(indeg[i] == 1) que.push(i);
    ll last = 0;

    while(!que.empty()) {
        ll v = que.front();
        que.pop();
        last = v;
        for(auto u : G[v]) {
            if(visited[v] == true) continue;
            indeg[u]--;
            if(indeg[u] == 1) {
                que.push(u);
            }
        }
    }
    // cout << last << "\n";
    visited.resize(n,false);
    vector<ll> dist(n,-1);
    vector<ll> parent(n,-2);
    auto dfs = [&](auto dfs,ll from,ll cnt, ll dia) ->bool {
        for(auto to : G[from]) {
            if(dist[to] == -1) {
                dist[to] = dist[from] + 1;
                parent
                if(dfs(dfs,to,cnt+1)) {
                    parent[to] = from;
                }
            }
        }
        chmax(dia,cnt);
        if(dia == cnt)
    };

    ll mx = -1;
    ll id = -1;
    rep(i,0,n) {
        if(chmax(mx,dist[i])) id = i;
    }

    ll ans = 0;
    rep(i,0,ssize(can)) {
        auto &[c,a,b] = can[i];
        if(i < (ssize(can)+1)/2) {
            ans += c;
        }else {
            ans += c*2;
        }
    }
    
    cout << ans << endl;
    
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

