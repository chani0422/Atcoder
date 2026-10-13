
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
ll n,m;
Graph G;
vector<ll> ans;
vector<ll> color(n,-1);
vector<ll> parent(n,-1);
vector<ll> state(n,0);
bool ok = false;
bool dfs(ll from, ll par) {
    state[from] = 1;
    for(auto to : G[from]) {
        if(ok) continue;
        if(to == par) continue;
        if(state[to] == 0) {
            parent[to] = from;
            color[to] = 1 - color[from];
            if(dfs(to,from)) {
                ok = true;
                return true;
            }
        }else if(state[to] == 1) {
            if(color[from] == color[to]) {
                ll cur = from;
                while(cur != to) {
                    ans.pb(cur);
                    cur = parent[cur];
                }
                ans.pb(cur);
                return true;
            }
        }
    }
    state[from] = 2;
    return false;
};
//E問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll Q; cin >> Q;
    while(Q--) {
        cin >> n >> m;
        ok = false;
        G.assign(n,vector<ll>(0));
        ans.clear();
        color.assign(n,-1);
        parent.assign(n,-1);
        state.assign(n,0);
        rep(i,0,m) {
            ll a,b; cin >> a >> b;
            a--;b--;
            G[a].pb(b);
            G[b].pb(a);
        }
        color[0] = 0;
        dfs(0,-1);
        if(ans.empty()) {
            cout << -1 << endl;
            continue;
        }else {
            cout << ans.size() << endl;
            for(auto x : ans) {
                cout << x+1 << " ";
            }
        }
        cout << endl;
    }
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
