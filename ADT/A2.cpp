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
using Graph = vector<vector<char>>;

//〇問題
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    set<string> st;
    ll n,m; cin >> n >> m;
    Graph s(n,vector<char>(n));
    rep(i,0,n)rep(j,0,n) cin >> s[i][j];

    rep(i,0,n-m+1) {
        rep(j,0,n-m+1) {
            string ss = "";
            rep(k,i,i+m) {
                rep(l,j,j+m) {
                    ss += s[k][l];
                }
            }
            st.insert(ss);
        }
    }
    cout << st.size() << endl;
    // For(st);
    return 0;
}