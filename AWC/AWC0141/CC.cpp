
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
//問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n,m; cin >> n >> m;
    vector<vector<vector<ll>>> dp(n,vector<vector<ll>>(m,vector<ll>(3)));
    Graph G(n,vector<ll>(m));
    rep(i,0,n)rep(j,0,m) cin >> G[i][j];
    
    rep(i,0,n) {
        rep(j,0,m) dp[i][j] = {-1,-1,-1};
    }

    if(n == 1) {
        cout << *max_element(nall(G[0])) << endl;
        return 0;
    }
    
    const int dy[3] = {-1,0,1};
    rep(j,0,m) {
        rep(k,0,3) {
            ll nj = j + dy[k];
            if(nj < 0 || m <= nj) continue;
            chmax(dp[1][nj][k],(G[0][j]+G[1][nj]));
        }
    }
    rep(i,2,n) {
        rep(j,0,m) {
            rep(k,0,3) {
                ll pj = j-dy[k];
                if(pj < 0 || m <= pj) continue;
                rep(pre,0,3) {
                    if(pre == 0 && k == 0) continue;
                    if(pre == 2 && k == 2) continue;
                    chmax(dp[i][j][k],dp[i-1][pj][pre]+G[i][j]);
                }



                // if(move == 0 || move == 3) {
                //     if(chmax(dp[i][j].fi,dp[max(0LL,i-1)][min(j+1,m-1)].fi+G[i][j])) dp[i][j].se = 2;
                //     if(chmax(dp[i][j].fi,dp[max(0LL,i-1)][j].fi+G[i][j])) dp[i][j].se = 1;
                // }else if(move == 2 || move == 3) {
                //     if(chmax(dp[i][j].fi,dp[max(0LL,i-1)][min(j+1,m-1)].fi+G[i][j])) dp[i][j].se = 0;
                //     if(chmax(dp[i][j].fi,dp[max(0LL,i-1)][j].fi+G[i][j])) dp[i][j].se = 1;
                // }else if(chmax(dp[i][j].fi,dp[max(0LL,i-1)][j].fi+G[i][j])) dp[i][j].se = 1;
            }
        }
    }

    ll mx = 0;
    rep(j,0,m) {
        rep(k,0,3) {
            chmax(mx,dp[n-1][j][k]);
        }
    }
    cout << mx << endl;

    // rep(i,0,n) {
    //     rep(j,0,m) {
    //         cout << dp[i][j].fi << " ";
    //     }
    //     cout << endl;
    // }

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
