#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
//E問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll h,w; cin >> h >> w;
    //上下左右移動用
    const int dx[4] = {1, 0, -1, 0};
    const int dy[4] = {0, 1, 0, -1};
    // const int dx[8] = {1, 1, 1, 0, 0, -1, -1, -1};
    // const int dy[8] = {1, 0, -1, 1, -1, 1, 0, -1};

    //型を確認して!
    vector<vector<ll>> G(h, vector<ll>(w));
    vector<vector<ll>> dist(h, vector<ll>(w, inf));
    //{コスト、x座標、y座標}
    using T = tuple<ll,ll,ll>;
    priority_queue<T,vector<T>,greater<T>> que;

    rep(i,0,h) {
        rep(j,0,w) {
            cin >> G[i][j];
        }
    }

    //単一始点
    ll start_x = 0;
    ll start_y = 0;
    ll start_cost = 0;
    if(G[start_x][start_y] != -1) {
        que.push({start_cost,start_x,start_y});
        dist[start_x][start_y] = start_cost;
    }else {
        cout << -1 << "\n";
        cout << "No\n";
        return 0;
    }


    while(!que.empty()){
        auto [c, x, y] = que.top();
        que.pop();
        if(dist[x][y] != c) continue;
        
        rep(dir,0,4){
            ll nx = x + dx[dir];
            ll ny = y + dy[dir];
            if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue; // 範囲外
            if(G[nx][ny] == -1) continue;
            ll d = G[nx][ny];
            // 非負コストのみを扱う
            // infを超える経路は扱わない(最短距離がinf未満になる時に限る)
            if(d < 0 || c > inf - d) continue;
            ll nc = c + G[nx][ny];
            //緩和
            if(chmin(dist[nx][ny],nc)) {
                que.push({nc,nx,ny});
            }
        }
    }

    //出力形式に合わせる
    ll Q; cin >> Q;
    while(Q--) {
        ll r,k; cin >> r >> k;
        r--;k--;
        cout << dist[r][k] << "\n";
    }

    cout << dist[h-1][w-1] << "\n";

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
