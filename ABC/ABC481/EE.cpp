
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
/*
Fなので書く
多重辺は考慮しなくていいでしょ
自己ループが起こる頂点に来たら10回同じ番号を足してmodとって配列に追加
kotatsugameさんが８分で解いてるから実装は重くなさそう。
なんかいけそう
*/
//F問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n,m; cin >> n >> m;
    vector<vector<pair<ll,char>>> G(n);
    vector<bool> Double(n,false);
    set<pair<ll,ll>> st;
    ll mod = 998244353;
    rep(i,0,m) {
        ll a,b; cin >> a >> b;
        char c; cin >> c;
        st.insert({a,b});
        a--;b--;
        if(a == b) Double[i] = true;
        if(st.contains({a,b})) continue;
        G[a].pb({b,c});
    }

    // Source: https://github.com/chani0422/Atcoder/blob/main/my-library/snipet.txt    
    vector<string> dist(n,"");
    dist[0] = "0";
    vector<bool> visited(n,false);
    queue<ll> que;
    visited[0] = true;
    que.push(0);
    
    while(!que.empty()){
        ll from = que.front();
        que.pop();
        string s = dist[from];

        for(auto [to,c] : G[from]){
            string t = "";
            if(visited[to] != false) continue; // 既訪問
            // visited[to] = true;
            //むり
            if(s == "0") t = c;
            else t = s + c;
            if(Double[to] == true) {
                while(ssize(t) != 10 || stoll(t) < mod) {
                    t += c;
                }
                ll v = stoll(t)%mod;
                dist[to] = to_string(v);
                visited[to] = true;
                continue;
            }
            string cur = to_string(stoll(t)%mod);
            ll u = stoll(dist[from]);
            ll v = stoll(cur)%mod;
            ll next = min(max(inf,u),v);
            dist[to] = to_string(next);
            que.push(to);
        }
    }
    
    rep(i,1,n) {
        cout << stoll(dist[i])%mod << " ";
    }
    cout << "\n";
    //From chanini library
    
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
