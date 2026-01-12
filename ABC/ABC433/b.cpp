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

//B問題
int main(void) {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ll n;cin >> n;
    bool ok = true;
    vector<ll> A(n);
    rep(i,0,n) cin >> A[i];
    rep(i,0,n) {
        if(!i) {cout << -1 << endl;continue;}
        ll tmp = i;
        ok = true;
        while(tmp > -1) {
            if(A[i] < A[tmp]) {
                cout << tmp+1 << endl;
                ok = false;
                break;
            }
            tmp--;
        }
        if(ok) cout << -1 << endl;
    }
    return 0;
}