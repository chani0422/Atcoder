#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=a;i<b;i++)
#define rrep(i,a,b) for(int i=a;i>=b;i--)
#define pb(x) push_back(x)
#define popb pop_back()
#define fi first
#define se second
#define yn(x) do { if(x) cout << "Yes\n"; else cout << "No\n"; } while(0)
#define nall(a) a.begin(),a.end()
#define rall(a) a.rbegin(),a.rend()
#define For(x) for(auto a : x)cout << a << " ";
using ll = long long; using ld = long double;
using ull = unsigned long long;
using Graph = vector<vector<ll>>;

//〇問題
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ll n; cin >> n;
    vector<pair<char,ll>> v(n);
    rep(i,0,n) {
        char c; cin >> c;
        ll a; cin >> a;
        pair<char,ll> p = {c,a};
        v.pb(p);
    }
    ll sum = 0;
    string s = "";
    for(auto x : v) {
        sum += x.se;
        if(sum > 100) {
            cout << "Too Long" << endl;
            return 0;
        }
        rep(i,0,x.se) {
            s += x.fi;
        }
    }
    cout << s << endl;
    return 0;
}