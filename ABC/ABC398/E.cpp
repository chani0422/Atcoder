
#if !__INCLUDE_LEVEL__
#include __FILE__


//解答コードここから

//E問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ll n; cin >> n;
    Graph G(n);
    vector<vector<bool>> edges(n,vector<bool>(n,false));
    rep(i,0,n-1) {
        ll a,b; cin >> a >> b;
        a--;b--;
        G[a].pb(b);
        G[b].pb(a);
        edges[a][b] = true;
        edges[b][a] = true;
    }
    queue<ll> que;
    vector<ll> color(n,-1);
    vector<ll> dist(n,-1);
    que.push(0);
    color[0] = 0;
    dist[0] = 0;

    while(!que.empty()) {
        ll from = que.front();
        que.pop();
        for(auto to : G[from]) {
            if(color[to] == -1) {
                color[to] = 1 - color[from];
                dist[to] = dist[from] + 1;
                que.push(to);
            }
        }
    }

    ll cnt0 = 0,cnt1 = 0;
    rep(i,0,n) {
        if(color[i] == 0) cnt0++;
        else cnt1++;
    }

    ll move = cnt1*cnt0 -(n-1);
    bool myturn = false;

    if(move%2 == 1) {
        cout << "First" << endl;
        myturn = true;
    }else {
        cout << "Second" << endl;
        myturn = false;
    }
    bool next = false;
    while(true){
        next = false;
        if(myturn) {
            rep(i,0,n) {
                rep(j,0,n) {
                    if(color[i] != color[j] && !edges[i][j]) {
                        edges[i][j] = true;
                        edges[j][i] = true;
                        next = true;
                        cout << i+1 << " " << j+1 << endl;
                        break;
                    }
                }
                if(next) break;
            }
        }else {
            ll i,j; cin >> i >> j;
            if(i == -1 && j == -1) {
                return 0;
            }
            i--;j--;
            edges[i][j] = true;
            edges[j][i] = true;
        }
        myturn = !myturn;
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

