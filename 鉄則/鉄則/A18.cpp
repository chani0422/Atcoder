#include <bits/stdc++.h>
using ll = long long; using ld = long double;
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
using Graph = vector<vector<bool>>;

//〇〇問題
int main(void) {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ll n,s; cin >> n >> s;
    vector<ll> S(n);
    Graph dp(n+1,vector<bool>(s+1,false));
    rep(i,0,n) cin >> S[i];
    
    dp[0][0] = true;
    rep(i,1,n) {
        rep(j,0,s) {
        if(dp[i-1][j]) dp[i][j] = true;
        else if(S[j] < s) dp[i][j+S[j]] = true;
        }
    }
    
    yn(dp[n-1][s-1]);
    rep(i,0,n) {
        rep(j,0,s) {
            cout << dp[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}