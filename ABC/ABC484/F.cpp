
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
// Source: https://github.com/chani0422/Atcoder/blob/main/my-library/snipet.txt
/* From chanini library*/
// 一点変更 → 区間和 + 区間最小値 + 区間最大値

// S : セグ木が持つ情報
struct S {
    ll minus;
    ll mono;
    ll num;
    bool ok;
    ll size;  // 区間の要素数
};

// 2区間を結合
S op(S a, S b) {
    //一種類の要素の数
    ll Mono = a.mono+b.mono;

    //一種類の値を引き継ぐ。
    ll num = (a.num != -1) ? a.num : b.num;

    //どちらもtrueか、どちらかが-1だけの区間か、全部同じならOK
    bool flag = ((a.ok && b.ok) && (a.num == -1 || b.num == -1 || a.num == b.num));

    return {
        a.minus+b.minus, //これ要らない
        Mono,
        num,
        flag,
        a.size+b.size
    };
}

// S側の単位元
S e() {
    return {
        0,
        0,
        -1,
        true,
        0
    };
}


//F問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n,Q; cin >> n >> Q;
    // Source: https://github.com/chani0422/Atcoder/blob/main/my-library/snipet.txt
    // 一点変更 → 区間和 + 区間最小値 + 区間最大値
    vector<S> A(n);
    //その区間がYesかNoかが分かれば良い
    //-1が何個ある？、-1じゃないやつが何個ある？、一種類ですか？を乗せる
    rep(i,0,n) {
        ll x; cin >> x;
        ll minus = x;
        ll mono = 1;
        if(x == -1) {
            minus = 1;
            mono = 0;
        }else minus = 0;
        A[i] = {minus, mono, x, true, 1};
    }
    
    segtree<S,op,e> seg(A);

    while(Q--) {
        ll t; cin >> t;
        if(t == 1) {
            ll i,x; cin >> i >> x;
            i--;
            ll minus = x;
            ll mono = 1;
            if(x == -1) {
                minus = 1;
                mono = 0;
            }else minus = 0;
            seg.set(i,S{minus,mono,x,true,1});
        }else {
            ll l,r; cin >> l >> r;
            l--;r--;
            // cout << "[" << l << "," << r << "]";
            // cout << seg.prod(l,r+1).minus << " ";
            // cout << seg.prod(l,r+1).mono << " ";
            // cout << seg.prod(l,r+1).num << " ";
            yn(seg.prod(l,r+1).ok);
            // cout << seg.prod(l,r+1).size << "\n";
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
