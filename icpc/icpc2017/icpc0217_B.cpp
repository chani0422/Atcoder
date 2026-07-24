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
    int sum = 0;
    int n;
    while(true) {
        cin >> n;
        sum = 0;
        if(n == 0) break;
        rep(i,0,n) {
            int a; cin >> a;
            if(sum+a > 300) continue;
            sum += a;
        }
    }
    cout << sum << "\n";
    return 0;
}