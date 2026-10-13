
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから

//AWC0127 Eの時、ダブルハッシュに進化しました。
struct RollingHash {
    static constexpr ll MOD1 = 1000000007;
    static constexpr ll MOD2 = 1000000009;
    static constexpr ll BASE = 911382323;
    vector<ll> hash1,hash2;
    vector<ll> power1,power2;
    RollingHash(const string& s) {
        ll n = s.size();
        hash1.assign(n+1,0);
        hash2.assign(n+1,0);
        power1.assign(n+1,1);
        power2.assign(n+1,1);

        for(int i = 0; i < n; i++) {
            ll x = static_cast<unsigned char>(s[i]) + 1;
            hash1[i + 1] = (hash1[i] * BASE + x) % MOD1;
            hash2[i + 1] = (hash2[i] * BASE + x) % MOD2;
            power1[i + 1] = power1[i] * BASE % MOD1;
            power2[i + 1] = power2[i] * BASE % MOD2;

        }
    }

    pair<ll,ll> get(int l, int r) const {
        ll result1 = hash1[r] - hash1[l] * power1[r-l] % MOD1;
        ll result2 = hash2[r] - hash2[l] * power2[r-l] % MOD2;
        if(result1 < 0) result1 += MOD1;
        if(result2 < 0) result2 += MOD2;
        return {result1,result2};
    }
};
//C問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll Q; cin >> Q;
    string s,t; cin >> s >> t;
    RollingHash S(s),T(t);
    ll szT = ssize(t);
    while(Q--) {
        ll l,r; cin >> l >> r;
        l--;r--;
        auto rh1 = S.get(l,r);
        auto rh2 = T.get(0,szT-1);
        yn(rh1 == rh2);
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
