
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//B問題
int main() {
    ios::sync_with_stdio(false);
    Graph bingo(3,vector<ll>(3));
    rep(i,0,3)rep(j,0,3) cin >> bingo[i][j];
    ll n; cin >> n;
    vector<ll> B(n);
    rep(i,0,n) cin >> B[i];
    rep(ii,0,n) {
        int a = B[ii];
    rep(i,0,3) {
        rep(j,0,3) {
            if(bingo[i][j] == a) bingo[i][j] = -1;
        }
    }
}
    ll cnt = 0;
    for(auto x : bingo) {
        ll tmp = 0;
        for(auto y : x) {
            if(y == -1) tmp++;
        }
        if(tmp == 3) cnt++;
    }
    Graph bingo2(3,vector<ll>(3));
    rep(i,0,3) {
        rep(j,0,3) {
            bingo2[i][j] = bingo[j][3-1-i];
        }
    }
    for(auto x : bingo2) {
        ll tmp = 0;
        for(auto y : x) {
            if(y == -1) tmp++;
        }
        if(tmp == 3) cnt++;
    }
    if(bingo[0][0]== -1&&bingo[1][1]==-1 && bingo[2][2] == -1) cnt++;
    if(bingo[2][0]== -1&& bingo[1][1] == -1&& bingo[0][2] == -1) cnt++;
    yn(cnt > 0);
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
#define For(x) for(auto a : (x)) cout << a << " ";
#define MIN(x) *min_element(nall(x))
#define MAX(x) *max_element(nall(x))

using ll = long long;
using ld = long double;
using ull = unsigned long long;

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
template<class T>
void For2(const T& a) {
    for (const auto& row : a) {
        for (const auto& v : row) cout << v << " ";
        cout << "\n";
    }
}

// === LIB_END ===
#endif

