
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
/* From chanini library*/
//区間加算 → 区間和 + 区間最小値 + 区間最大値

// S : セグ木が持つ値
struct S {
    ll sum;   // 区間和
    ll mn;    // 区間最小値
    ll mx;    // 区間最大値
    ll size;  // 区間の要素数
};

// F : 遅延させる更新 → +x
using F = long long;

// 2区間を結合
S op(S a, S b) {
    return {
        a.sum + b.sum,
        min(a.mn, b.mn),
        max(a.mx, b.mx),
        a.size + b.size
    };
}

// S側の単位元
S e() {
    return {
        0,      // sum
        inf,    // mn
        -inf,   // mx
        0       // size
    };
}

// 更新 f をデータ x に適用
// 区間全体に +f
S mapping(F f, S x) {

    // 空区間なら何もしない
    if(x.size == 0) return x;

    return {
        x.sum + f * x.size,
        x.mn + f,
        x.mx + f,
        x.size
    };
}

// 更新を合成
// +g のあとに +f → +(f+g)
ll composition(F f, F g) {return f + g;}

// F側の単位元 +0
ll id(){return 0;}

/*
    【使い方】
    【できること】
    ・区間加算
    ・区間和
    ・区間最小値
    ・区間最大値
    ・一点取得
    ・一点変更


   // [l,r) に +x
    seg.apply(l, r, x);

   // p に +x
    seg.apply(p, x);

   // [l,r) の情報取得
    S res = seg.prod(l, r);
   res.sum;   // 区間和
   res.mn;    // 区間最小値
   res.mx;    // 区間最大値
   res.size;  // 区間サイズ

   // 全区間の情報
    S res = seg.all_prod();

   // p 番目の情報
    S res = seg.get(p);

   // p 番目を x に変更
    seg.set(p, {x, x, x, 1});
*/

//D問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n,Q; cin >> n >> Q;
    vector<S> A(n);
    rep(i,0,n) {
        A[i] = {0,0,0,1};
    }
    vector<vector<pair<ll,ll>>> q(Q+1);
    rep(i,0,Q) {
        ll l,r,x; cin >> l >> r >> x;
        l--;r--;
        q[x].pb({l,r});
    }
    lazy_segtree<S,op,e,F,mapping,composition,id> seg(A);
    rep(i,0,Q+1) {
        sort(nall(q[i]));
        if(q[i].empty()) continue;
        ll l = q[i][0].fi;
        ll r = q[i][0].se;
        ll a,b;
        rep(j,1,ssize(q[i])) {
            auto [L,R] = q[i][j];
            if(L <= r) chmax(r,R);
            else {
                l = L;
                r = R;
                seg.apply(l,r+1,1);
            }
        }
        seg.apply(l,r+1,1);
    }
    rep(i,0,n) {
        cout << seg.get(i).sum << " ";
    }
    cout << "\n";
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
