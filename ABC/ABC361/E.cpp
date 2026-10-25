
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
/*
GPTに「直径は辺の本数ではなく、重みの合計で求めます。」というヒントをもらった。
木ということは根がある。根から葉に向かってBFSして1番目と2番目に重い経路の重み＋それ以外の辺の重み＊２が答えになるのでは？
というかそれでしょ
制約的にすべての親を記録できるか
じゃあそれを実装します。

ん？別に親記録しなくてもいいな
重みが1位と2位だけわかればいいやん
一回目の提出は11個WA
んー、方針は合ってるけど、なんかのパターンで通らないっぽい
流石に嘘解法ではないと思う

わかった、今の実装だと根を一回だけ通るルートが根を通らないルートから選ばれることがある。
根から各葉につながる経路かつ、根から出るルートが2種類ないといけない。
実装はABC終わってからやります

実装終わったけど、提出したらWA2個...
流石にオーバーフローの可能性はないでしょう
そいえば、根は一意に定まるのか？
一番大きい辺を共有する頂点のどちらかを根にしたほうが良いのか？
んー、でも結局総和がデカい経路を中心にしたいからなあ
やっぱり一番最初に考えた最大全域木でどうこうするのがいいのか？
もう諦めたいけど、ここまで来たら自力で最後まで通したい
反例探してきます。

一旦ullにして出してみる。
変わんなかった

Nが2個の時だけ処理を分けたがWA2個から動かん
え、嘘解法なんですか？もしかして方針から間違えてるのか？

流石にこの問題に時間かけすぎなので諦めます。
原因は根の選び方が一意ではないに一票

そいえば最長経路を主要道路にしたいんだった。時間かけすぎたせいか本来の目的見失ってたかも
二回DFSして一番遠い2頂点間の距離＋それ以外の辺×2が答えかー
木の直径は考えて2回DFSすればいいってのは分かってたけど、重み付きのグラフにおいて最長経路をどうあぶりだすかわからず一旦その考えは捨ててしまったんだよなー
良い問題でした。ありがとうございます。
*/
//E問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ll n; cin >> n;
    Graph G(n);
    vector<ll> indeg(n,0);
    map<pair<ll,ll>,ll> edges;
    ll ans = 0;
    rep(i,0,n-1) {
        ll a,b,c; cin >> a >> b >> c;
        a--;b--;
        indeg[a]++;
        indeg[b]++;
        G[a].pb(b);
        G[b].pb(a);
        edges[{a,b}] = c;
        edges[{b,a}] = c;
        ans += c;
    }

    vector<ll> dist(n,-1);
    dist[0] = 0;

    auto dfs = [&](auto dfs,ll from)->void {
        for(auto to : G[from]) {
            if(dist[to] != -1) continue;
            dist[to] = dist[from] + edges[{from,to}];
            dfs(dfs,to);
        }
    };

    ll first = -1;
    dfs(dfs,0);
    first = max_element(nall(dist))-dist.begin();
    dist.assign(n,-1);
    dist[first] = 0;
    dfs(dfs,first);
    ll dia = *max_element(nall(dist));

    cout << ans*2 - dia << "\n";
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

