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
using ll = long long; 
using ld = long double;
using ull = unsigned long long;
using Graph = vector<vector<ll>>;

//B問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n; cin >> n;
    ll r = sqrt(n);
    vector<ll> A;
    vector<ll> mp(n+1,0);
    rep(x,1,r+1) {
        rep(y,2,r+1) {
            if(x >= y) continue;
            if(ll(x*x+y*y) > n) break;
            mp[ll(x*x+y*y)]++;
            // cout << x << ":" << y << endl;
    }
}

    ll cnt = 0;
    rep(i,0,n+1) {
        if(mp[i] == 1) {
            A.pb(i);
            cnt++;
        }
    }

    
    cout << cnt << endl;
    For(A);
    return 0;
}
