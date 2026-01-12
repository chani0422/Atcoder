
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
using Graph = vector<vector<ll>>;

//A問題
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ll n,r; cin >> n >> r;
    if(n >= 10) {
        cout << r << endl;
        return 0;
    }
    cout << r - (100*(10-n)) << endl;
    return 0;
}

