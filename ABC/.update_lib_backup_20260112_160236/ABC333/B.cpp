
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//B問題
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    char s1,s2,t1,t2; cin >> s1 >> s2 >> t1 >> t2;
    int S1,S2,T1,T2;
    if(s1 == 'A') S1 = 0;
    else if(s1 == 'B') S1 = 1;
    else if(s1 == 'C') S1 = 2;
    else if(s1 == 'D') S1 = 3;
    else S1 = 4;
    if(t1 == 'A') T1 = 0;
    else if(t1 == 'B') T1 = 1;
    else if(t1 == 'C') T1 = 2;
    else if(t1 == 'D') T1 = 3;
    else T1 = 4;
    if(s2 == 'A') S2 = 0;
    else if(s2 == 'B') S2 = 1;
    else if(s2 == 'C') S2= 2;
    else if(s2 == 'D') S2 = 3;
    else S2 = 4;
    if(t2 == 'A') T2 = 0;
    else if(t2 == 'B') T2 = 1;
    else if(t2 == 'C') T2 = 2;
    else if(t2 == 'D') T2 = 3;
    else T2 = 4;
    int g[5][5] = {
        {0,1,2,2,1},
        {1,0,1,2,2},
        {2,1,0,1,2},
        {2,2,1,0,1},
        {1,2,2,1,0}
    };
    int ans1 = g[S1][S2], ans2 = g[T1][T2];
    // if(ans1 == ans2) cout << "Yes" << endl;
    // else cout << "No" << endl;
    if(ans1 == ans2 ? "Yes" : "No");
    yn(ans1 == ans2);
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

