
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//D問題
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);

    int h, w; cin >> h >> w;
    int sx, sy, gx, gy; cin >> sx >> sy >> gx >> gy;
    --sx; --sy; --gx; --gy;

    vector<string> g(h);
    rep(i,0,h) cin >> g[i];

    //上下左右移動用
    const int dx[4] = {1, 0, -1, 0};
    const int dy[4] = {0, 1, 0, -1};

    vector<vector<ll>> dist(h, vector<ll>(w, INF));
    deque<pair<int,int>> q;

    dist[sx][sy] = 0;
    q.push_front({sx, sy});

    while(!q.empty()){
        auto [x, y] = q.front();
        q.pop_front();

        rep(dir,0,4){
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue; // 範囲外
            if(g[nx][ny] == '#') continue; // 壁
            if(dist[nx][ny] <= dist[x][y]) continue; // 既訪問
            dist[nx][ny] = dist[x][y];
            q.push_front({nx, ny});
        }

            rep(i,0,5) {
                rep(j,0,5) {
                    int nx = x-2+i;
                    int ny = y-2+j;
                    if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue; // 範囲外
                    if(g[nx][ny] == '#') continue; // 壁
                    if(dist[nx][ny] <= dist[x][y]+1) continue; // 既訪問
                    dist[nx][ny] = dist[x][y] + 1;
                    q.push_back({nx, ny});
                }
            }
    }

    if(dist[gx][gy] == INF) {
            out(-1);
        }else cout << dist[gx][gy] << endl;
    
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

