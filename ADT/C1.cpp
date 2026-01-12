#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=a;i<b;i++)
#define rrep(i,a,b) for(int i=a;i>=b;i--)
#define pb(x) push_back(x)
#define popb() pop_back()
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
    vector<ll> pos;
    rep(i,0,60) {
        if(n & (1LL<<i)) pos.pb(i);
    }
    ll sz = pos.size();
    for(ll mask = 0; mask < (1LL<<(sz));mask++) {
        ll x = 0;
        rep(i,0,sz) {
            if(mask & (1LL<<i)) {
                x |= (1LL<<pos[i]);
            }
        }
        cout << x << endl;
    }
    return 0;
}