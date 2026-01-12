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
    ll n;cin >> n;
    vector<ll> p(n);
    rep(i,0,n) cin >> p[i];
    ll Q;cin >> Q;
    rep(t,0,Q) {
        ll a,b; cin >> a >> b;
        rep(i,0,n) {
            if(p[i] == a) {
            cout << a << endl;
            break;
        }
        
        if(p[i] == b) {
            cout << b << endl;
            break;
        }
        }
    }
    return 0;
}