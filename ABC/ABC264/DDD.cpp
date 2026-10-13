
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
/* From chanini library*/
// Fenwick Tree (BIT)
// できること
// ・一点加算     add(p, x)     : A[p] += x
// ・区間和       sum(l, r)     : A[l] + ... + A[r-1]
// ・一点取得     get(p)
// ・一点代入     set(p, x)

template<class T>
struct FenwickTree {
    int n;
    vector<T> bit;

    FenwickTree(int n) : n(n), bit(n + 1, 0) {}

    // vectorから構築
    FenwickTree(const vector<T>& a) : n(a.size()), bit(n + 1, 0) {
        for(int i = 0; i < n; i++) {
            add(i, a[i]);
        }
    }

    // A[p] += x
    void add(int p, T x) {
        p++; // 内部は1-indexed
        while(p <= n) {
            bit[p] += x;
            p += p & -p;
        }
    }

    // A[0] + ... + A[r-1]
    T sum(int r) const {
        T res = 0;

        while(r > 0) {
            res += bit[r];
            r -= r & -r;
        }

        return res;
    }

    // A[l] + ... + A[r-1]
    T sum(int l, int r) const {
        return sum(r) - sum(l);
    }

    // A[p]を取得
    T get(int p) const {
        return sum(p, p + 1);
    }

    // A[p] = x
    void set(int p, T x) {
        add(p, x - get(p));
    }
};
// https://output-zakki.com/inversion_number/
//D問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s; cin >> s;
    string t = "atcoder";
    map<ll,ll> mp;
    vector<ll> A(7);
    ll id = 0;
    for(char c : t) {
        mp[c] = id;
        id++;
    }
    rep(i,0,ssize(s)) {
        A[i] = mp[s[i]];
    }
    FenwickTree<ll> bit(7);
    ll ans = 0;
    rep(i,0,ssize(s)) {
        //全体 = x以下 + xより大きい
        //xより大きい = 全体 - x以下
        ll cur = A[i];
        // ans += i-bit.sum(0,cur+1);
        ans += bit.sum(cur+1,7);
        bit.add(cur,1);
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

