
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//D問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s; cin >> s;
    vector<pair<char,ll>> RLE;
    vector<ll> res(int(s.size()),0);
    char c = s[0];
    ll cnt = 1;
    vector<ll> bor;
    rep(i,1,int(s.size())) {
        if(s[i] == 'R' && c == 'R' || s[i] == 'L' && c == 'L') cnt++;
        else {
            RLE.push_back({c,cnt});
            bor.pb(i-1);
            cnt = 1;
            c = s[i];
        }
    }
    RLE.push_back({c,cnt});
    
    rep(i,0,(int)RLE.size()-1) {
        if(RLE[i].fi == 'R' && RLE[i+1].fi == 'L') {
            ll r = RLE[i].se;
            ll l = RLE[i+1].se;
            res[bor[i]] = (r+1)/2 + l/2;
            res[bor[i]+1] = r/2 + (l+1)/2;
        }
    }
    
    For(res);
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

