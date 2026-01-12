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
    ld r = sqrt(n);
    ll cnt = 0;
    vector<ll> A;
    set<ll> ans;
    set<ll> B;
    rep(i,1,n+1) {
        B.insert(i);
    }
    rep(N,0,r) {
    rep(x,1,r) {
        rep(y,0,r) {
            if(x >= y) continue;
            
            A.pb(x*x+y*y);
        }
    }
    }
    
    vector<ll> visited(n+1,0);
    rep(i,0,A.size()) {
        auto it = lower_bound(B.begin(),B.end(),A[i]);
        if(*it == A[i]) {
        visited[*it-1]++;
        ans.insert(A[i]);
        }
    }

    // cout << ans.size() << endl; 
    // for(auto x : ans) {
    //     cout << x << " ";
    // }
    rep(i,0,visited.size()) {
        if(visited[i] > 1) continue;
        cout << i+1 << " ";
    }
    cout << endl;
    // cout << A.size() << endl;
    // For(A)
    return 0;
}