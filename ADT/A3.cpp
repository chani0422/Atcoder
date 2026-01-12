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
    ll w,b; cin >> w >> b;
    while(1) {
        if(w <= 0 || b <= 0) break;
        if(w-1 == b || b-1 == w || b == w) {
            cout << "Yes\n";
            return 0;
        }
        if(w > b) {
            w-=2;b--;
        }else b-=2;w--;
        // cout << w << ":" << b << endl;
    }
    cout << "No\n";
    return 0;
}