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
    while(true) {
        ll n,m; cin >> n >> m;
        if(n == 0 && m == 0) return 0;
        vector<ll> A(n),T;
        rep(i,0,n) cin >> A[i];
        rep(i,0,n) {
            rep(j,i+1,n) {
                ll tmp = A[i] + A[j];
                if(tmp <= m) T.pb(tmp);
            }
        }
        sort(nall(T));
        if(T.empty()) {
            cout << "NONE" << "\n";
            continue;
        }
        
        cout << T[T.size()-1] << "\n";
    }
    return 0;
}
