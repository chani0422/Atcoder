
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
/* From chanini library*/
// 一点変更 → 区間和 + 区間最小値 + 区間最大値

// S : セグ木が持つ情報
struct S {
    ll zero;
    ll one;
    ll two;
    ll tentou;
};

// 2区間を結合
S op(S a, S b) {
    return {
        a.zero+b.zero,
        a.one+b.one,
        a.two+b.two,
        a.tentou+b.tentou+a.one*b.zero+a.two*(b.one+b.zero)
    };
}

// S側の単位元
S e() {
    return {
        0,0,0,0
    };
}

/*
    【使い方】
    【できること】
    ・一点変更
    ・一点取得
    ・区間和
    ・区間最小値
    ・区間最大値


   // p 番目を x に変更
    seg.set(p, {x, x, x, 1});

   // p 番目の情報取得
    S res = seg.get(p);
    res.sum;
    res.mn;
    res.mx;
    res.size;

   // [l,r) の情報取得
    S res = seg.prod(l, r);
    res.sum;   // 区間和
    res.mn;    // 区間最小値
    res.mx;    // 区間最大値
    res.size;  // 区間サイズ

   // 全区間の情報
    S res = seg.all_prod();


    【注意】
    普通のセグ木なので区間更新はできない。
    更新は seg.set() による一点変更。

    区間はすべて半開区間 [l,r)。
*/

// https://output-zakki.com/inversion_number/
//左にある1と右にある0の個数の積と、
//左にある2と右にある0と1の個数の積の合計が転倒数になる。
//上のリンク先の図で視覚的に理解できる

//問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n, Q; cin >> n >> Q;
    vector<ll> A(n);
    rep(i,0,n) cin >> A[i];
    vector<S> B(n);
    // ll mx = -inf;
    // B[0] = 0;
    rep(i,0,n) {
        ll cur = A[i];
        ll a0 = (cur == 0 ? 1:0);
        ll a1 = (cur == 1 ? 1:0);
        ll a2 = (cur == 2 ? 1:0);
        B[i] = {a0,a1,a2,0};
    }

    // rep(i,0,n) {
    //     cout << bit.get(i) << " ";
    // }
    // cout << "\n";
    
    segtree<S,op,e> seg(B);
    while(Q--) {
        ll q; cin >> q;
        if(q == 1) {
            ll i,x; cin >> i >> x;
            i--;
            ll a0 = (x == 0 ? 1:0);
            ll a1 = (x == 1 ? 1:0);
            ll a2 = (x == 2 ? 1:0);
            seg.set(i,S{a0,a1,a2,0});
        } else {
            ll l,r; cin >> l >> r;
            l--;r--;
            ll ans = seg.prod(l,r+1).tentou;
            cout << ans << "\n";
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