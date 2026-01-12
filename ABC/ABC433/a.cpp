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

//A問題
int main(void) {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ld x,y,z; cin >> x >> y >> z;
    rep(i,0,100) {
        if(x/y == z) {
            cout << "Yes" << endl;
            return 0;
        }
        x++;
        y++;
    }
    cout << "No" << endl;
    return 0;
}