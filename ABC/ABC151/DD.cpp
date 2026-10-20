
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
// Source: https://github.com/chani0422/Atcoder/blob/main/my-library/snipet.txt
/*
   Grid版 Warshall-Floyd
   '.' : 通行可能
   '#' : 壁
   上下左右に移動可能（コスト1）

   計算量 : O((H*W)^3)
   空間計算量 : O((H*W)^2)

   dist[u][v] : uからvへの最短距離
   prev[u][v] : 最短経路でvの直前の頂点
*/

// Grid版ワーシャルフロイド
void warshall_floyd_grid(
    const Grid& G,
    vector<vector<ll>>& dist,
    vector<vector<ll>>& prev
) {
    ll h = ssize(G);
    ll w = ssize(G[0]);
    ll V = h*w;

    dist.assign(V,vector<ll>(V,inf));
    prev.assign(V,vector<ll>(V,-1));

    const int dx[4] = {1,0,-1,0};
    const int dy[4] = {0,1,0,-1};

    // 2次元座標を1次元の頂点番号に変換
    auto id = [&](ll x,ll y) {
        return x*w+y;
    };

    // 初期化
    rep(i,0,h) {
        rep(j,0,w) {
            if(G[i][j] == '#') continue;

            ll u = id(i,j);
            dist[u][u] = 0;
            prev[u][u] = u;

            rep(dir,0,4) {
                ll nx = i+dx[dir];
                ll ny = j+dy[dir];

                if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue;
                if(G[nx][ny] == '#') continue;

                ll v = id(nx,ny);
                dist[u][v] = 1;
                prev[u][v] = u;
            }
        }
    }

    // Warshall-Floyd
    rep(k,0,V) {
        rep(i,0,V) {
            if(dist[i][k] == inf) continue;

            rep(j,0,V) {
                if(dist[k][j] == inf) continue;

                if(chmin(dist[i][j],dist[i][k]+dist[k][j])) {
                    prev[i][j] = prev[k][j];
                }
            }
        }
    }
}

// 最短経路復元（座標を返す）
vector<pair<ll,ll>> get_path_grid(
    const vector<vector<ll>>& prev,
    ll sx,ll sy,ll gx,ll gy,ll w
) {
    ll s = sx*w+sy;
    ll t = gx*w+gy;

    vector<pair<ll,ll>> path;

    if(prev[s][t] == -1) return path;

    for(ll cur=t; cur!=s; cur=prev[s][cur]) {
        path.push_back({cur/w,cur%w});
    }

    path.push_back({sx,sy});
    reverse(nall(path));

    return path;
}
//D問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ll h,w; cin >> h >> w;
    Grid G(h,vector<char>(w));
    rep(i,0,h)rep(j,0,w) cin >> G[i][j];
    vector<vector<ll>> dist(h,vector<ll>(w));
    vector<vector<ll>> prev(h,vector<ll>(w));

    warshall_floyd_grid(G,dist,prev);
    ll V = h*w;
    ll ans = -inf;
    rep(i,0,V) {
        rep(j,i+1,V) {
            if(dist[i][j] == inf) continue;
            chmax(ans,dist[i][j]);
        }
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

