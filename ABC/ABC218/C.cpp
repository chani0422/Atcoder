
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//C問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ll n; cin >> n;
    vector<vector<char>> s(n,vector<char>(n)),t(n,vector<char>(n));
    rep(i,0,n)rep(j,0,n) cin >> s[i][j];
    rep(i,0,n)rep(j,0,n) cin >> t[i][j];

    vector<pair<ll,ll>> ps,pt;
    rep(i,0,n)rep(j,0,n) {
        if(s[i][j] == '#') ps.pb({i,j});
    }
    rep(i,0,n)rep(j,0,n) {
        if(t[i][j] == '#') pt.pb({i,j});
    }
    
    if(ps.size() != pt.size()) {
        yn(0);
        return 0;
    }

    auto normalize = [&](vector<pair<ll,ll>> v) {
        ll min_i = inf, min_j = inf;
        for(auto x : v) {
            chmin(min_i,x.fi);
            chmin(min_j,x.se);
        }
        for(auto &x : v) {
            x.fi -= min_i;
            x.se -= min_j;
        }
        sort(nall(v));
        return v;
    };

    auto ns = normalize(ps);

    rep(i,0,4) {
        auto nt = normalize(pt);
        if(nt == ns) {
            cout << "Yes\n";
            return 0;
        }
        vector<vector<char>> r(n,vector<char>(n));
        rep(i,0,n) {
            rep(j,0,n) {
                r[n-1-j][i] = t[i][j];
            }
        }
        t = r;
        pt.clear();
        rep(i,0,n) {
            rep(i,0,n) {
                if(t[i][j] == '#') pt.pb({i,j});
            }
        }
    }

    
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
#define pll = pair<ll,ll>

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

