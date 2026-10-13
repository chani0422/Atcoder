
#if !__INCLUDE_LEVEL__
#include __FILE__

//From chanini library
//解答コードここから
//問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n,Q; cin >> n >> Q;

    Graph G(n+1);

    vector<ll> dist(n+1,inf);
    vector<ll> pref(n*2+2,0);
    vector<ll> A(n);
    rep(i,0,n) {
        ll a; cin >> a;
        G[i].pb({(i+1)%n,a});
        A[i] = a;
        G[{(i+1)%n}].pb({i,a});
        pref[i+1] = pref[i] + a;
    }
    rep(i,n,n*2) {
        ll a = pref[i];
        ll b = A[i-n];
        pref[i+1] = a+b;
    }
    rep(i,0,n) {
        ll a; cin >> a;
        G[i].pb({n,a});
        G[n].pb({i,a});
    }
    
    priority_queue<pair<ll,ll>,vector<pair<ll,ll>>,greater<pair<ll,ll>>> pq;
    ll start_v = n;
    pq.push({0,start_v});
    dist[start_v] = 0;

    while(!pq.empty()) {
        auto [d,v] = pq.top();
        pq.pop();
        if(d != dist[v]) continue;

        for(auto [to,c] : G[v]) {
            if(dist[to] > d + c) {
            dist[to] = d + c;
            pq.push({dist[to],to});
            }
        }
    }


    while(Q--) {
        ll s,t; cin >> s >> t;
        s--;t--;
        ll a = (dist[s]+dist[t]);
        if(t == n) {
            cout << dist[s] << "\n";
            continue;
        }
        // s++;t++;
        ll b = min(pref[t]-pref[s],pref[s+n]-pref[t]);
        cout << min(a,b) << "\n";
    }
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

using Graph= vector<vector<pair<ll,ll>>>;
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
