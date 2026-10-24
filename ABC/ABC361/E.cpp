
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
わかった、根を今の実装だと一回だけ通るルートが根を通らないルートから選ばれることがある。
根から各葉につながる経路かつ、根から出るルートが2種類ないといけない。
実装はABC終わってからやります

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

    queue<ll> que;
    vector<ll> visited(n,false);
    rep(i,0,n) if(indeg[i] == 1) que.push(i);
    ll root = 0;

    while(!que.empty()) {
        ll v = que.front();
        que.pop();
        root = v;
        for(auto u : G[v]) {
            if(visited[v] == true) continue;
            indeg[u]--;
            if(indeg[u] == 1) {
                que.push(u);
            }
        }
    }
    // cout << root << "\n";
    visited.resize(n,false);
    vector<ll> dist(n,-1);
    que.push(root);
    dist[root] = 0;
    visited[root] = true;
    
    while(!que.empty()) {
        ll from = que.front();
        que.pop();
        for(auto to : G[from]) {
            if(dist[to] == -1) {
                dist[to] = dist[from] + edges[{from,to}];
                que.push(to);
            }
        }
    }

    ll mx1 = 0,mx2 = 0;
    rep(i,0,n) {
        ll tmp = mx1;
        if(chmax(mx1,dist[i])) {
            mx2 = tmp;
        }else {
            chmax(mx2,dist[i]);
        }
    }


    cout << ans*2 - (mx1 + mx2) << "\n";
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

