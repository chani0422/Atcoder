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
    ll n,a,b; cin >> n >> a >> b;
    string s = ".";
    rep(i,0,n) {
        rep(j,0,a)  {
            rep(k,0,n) {
                rep(l,0,b) {
                    if(i%2==0) {
                        if(k%2==0) cout << ".";
                        else cout << "#";
                    }else {
                        if(k%2==1) cout << ".";
                        else cout << "#";
                    }
                }
            }
            cout << "\n";
        }
    }
    return 0;
}