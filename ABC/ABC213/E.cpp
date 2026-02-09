
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//E問題
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);

    int h, w; cin >> h >> w;

    vector<string> g(h);
    rep(i,0,h) cin >> g[i];

    // 徒歩（cost 0）用：上下左右
    const int dx[4] = {1, 0, -1, 0};
    const int dy[4] = {0, 1, 0, -1};

    const int INF = 1e9;
    vector<vector<ll>> dist(h, vector<ll>(w, INF));
    // vector<vector<pair<int,int>>> parent(h, vector<pair<int,int>>(w, {-1,-1}));

    deque<pair<int,int>> dq;

    dist[0][0] = 0;
    dq.push_front({0,0});

    while(!dq.empty()){
        auto [x, y] = dq.front();
        dq.pop_front();

        ll cur = dist[x][y];

        //cost 0
        rep(dir,0,4){
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue;
            if(g[nx][ny] == '#') continue;

            if(dist[nx][ny] > cur) {
                dist[nx][ny] = cur;
                dq.push_front({nx, ny});
                // parent[nx][ny] = {x, y};
        }
        }

        //cost 1
        rep(i,-2,3){
            rep(j,-2,3){
                if(abs(i) == abs(j)) continue;
                int nx = x + i;
                int ny = y + j;

                if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue;
                // if(g[nx][ny] == '#') continue;

                if(dist[nx][ny] > cur + 1) {
                dist[nx][ny] = cur + 1;
                dq.push_back({nx, ny});
                // parent[nx][ny] = {x, y};
                }
            }
        }
    }

    if(dist[h-1][w-1] == INF) cout << -1 << "\n";
    else cout << dist[h-1][w-1] << "\n";

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

