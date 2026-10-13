
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから
//問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n,m,k; cin >> n >> m >> k;
    vector<ll> H(n);
    rep(i,0,n) cin >> H[i];

    vector<vector<pair<ll,ll>>> G(n);
    rep(i,0,m) {
        ll a,b; cin >> a >> b;
        a--;b--;
        ll c = abs(H[a]-H[b]);
        if(c > k) continue;
        G[a].emplace_back(b,c);
        G[b].emplace_back(a,c);
    }
    vector<ll> dist(n,inf);
    dist[0] = 0;

    priority_queue<pair<ll,ll>,vector<pair<ll,ll>>,greater<pair<ll,ll>>> pq;
    ll start_v = 0;
    pq.push({0,start_v});
    dist[start_v] = 0;

    while(!pq.empty()) {
        auto [d,v] = pq.top();
        pq.pop();
        if(d != dist[v]) continue;

        for(auto [to,c] : G[v]) {
            if(dist[to] > d + c) {
            // if(abs(d-c) > k) continue;
            dist[to] = d + c;
            pq.push({dist[to],to});
            }
        }
    }

    //出力
    if(dist[n-1] == inf) {
        cout << -1 << endl;
        return 0;
    }

    cout << dist[n-1] << endl;


    // while(!que.empty()) {
    //     ll from = que.front();
    //     que.pop();
    //     for(auto to : G[from]) {
    //         if(dist[to] != -1) continue;
    //         if(abs(H[from]-H[to]) > k) continue;
    //         que.push(to);
    //         dist[to] = dist[from] + abs(H[from]-H[to]);
    //     }
    // }
    // if(dist[n-1] == -1) {
    //     cout << -1 << endl;
    //     return 0;
    // }
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

