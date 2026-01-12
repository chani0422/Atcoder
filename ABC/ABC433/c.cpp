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

//C問題
int main(void) {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    string s; cin >> s;
    queue<ll> index;
    rep(i,0,s.size()-1) {
        if(s[i] == s[i+1]-1) index.push(i);
    }
    ll ans = 0;
    while(!index.empty()) {
        ll v = index.front();
        int l = v, r = v+1;
        while(s[l] == s[r]-1) {
            ans++;
            l--;
            r++;
            if(l == -1 || r == s.size()) break;
            if(s[l+1] != s[l] || s[r-1] != s[r]) break;
        }
        index.pop();
    }
    cout << ans << endl;
    return 0;
}