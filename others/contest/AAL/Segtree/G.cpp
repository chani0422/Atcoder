
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
using mint = modint998244353;
const ll MOD = 998244353;
/* From chanini library*/
//区間加算 → 区間和 + 区間最小値 + 区間最大値

// S : セグ木が持つ値
struct S {
    mint a1;
    mint a2;
    mint a3;
    mint a4;
    ll size;  // 区間の要素数
};

// F : 遅延させる更新 → +x
using F = mint;

// 2区間を結合
S op(S a, S b) {
    return {
        a.a1+b.a1,
        a.a2+b.a2,
        a.a3+b.a3,
        a.a4+b.a4,
        a.size + b.size
    };
}

// S側の単位元
S e() {
    return {
        0,      // sum
        0,    // mn
        0,   // mx
        0       // size
    };
}

// 更新 f をデータ x に適用
// 区間全体に +f
S mapping(F f, S x) {
    // 空区間なら何もしない
    if(x.size == 0) return x;
    mint f2 = f*f;
    mint f3 = f2*f;
    mint f4 = f3*f;
    mint sz = x.size;
    return {
        x.a1+f*sz,
        x.a2 + 2*f*x.a1 + f2*sz,
        x.a3 + 3*f*x.a2 + 3*f2*x.a1 + f3*sz,
        x.a4 + 4*f*x.a3 + 6*f2*x.a2 + 4*f3*x.a1 + f4*sz,
        x.size
    };
}

// 更新を合成
// +g のあとに +f → +(f+g)
F composition(F f, F g) {return f + g;}

// F側の単位元 +0
F id(){return 0;}

//問題
/*
//繰り返し二乗法は使うとして、どうやって(l,r]の範囲を一気に計算するか
//kは1<= k <= 4でした！！！！！
//でもバイトまで時間ない！！
//このくらいのモノイド設計だったらわんちゃんできそうなのに！！
//なんでバイトいれたんや
//できるところまで考察して記念提出するかも
//16:30そろそろバイトなので家で増す。
//1~4までのやつを式を展開してそれをmappingに適用させる。
//ちょっとまえmappingをいじりまくる問題やったからその容量で
・/4だったら(A_i + f)^4を展開してごちゃごちゃする
//powでいけるか
//諦めました。こっから解説AC
//シグマ付いたまま式を展開するとその区間内の
//それぞれの∑A_i^(1~4)がでてくるからそれを利用する
*/
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n, Q; cin >> n >> Q;
    //区間加算 → 区間和 + 区間最小値 + 区間最大値
    vector<S> A(n);
    rep(i,0,n) {
        ll a; cin >> a;
        mint x = a;
        A[i] = {x,x*x,x*x*x,x*x*x*x,1};
    }
    
    lazy_segtree<S,op,e,F,mapping,composition,id> seg(A);
    
    while(Q--) {
        ll q; cin >> q;
        if(q == 1) {
            ll l,r,x; cin >> l >> r >> x;
            l--;r--;
            seg.apply(l,r+1,x);
        }else {
            ll l,r,k; cin >> l >> r >> k;
            l--;r--;
            if(k == 1) cout << seg.prod(l,r+1).a1.val() << "\n";
            if(k == 2) cout << seg.prod(l,r+1).a2.val() << "\n";
            if(k == 3) cout << seg.prod(l,r+1).a3.val() << "\n";
            if(k == 4) cout << seg.prod(l,r+1).a4.val() << "\n";
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