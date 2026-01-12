#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=a;i<b;i++)
#define rrep(i,a,b) for(int i=a;i>=b;i--)
#define pb(x) push_back(x)
#define popb() pop_back()
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
    ll n; cin >> n;
    set<ll> st;
    ll can = 0,cur = 0;
    
    rep(i,0,n) {
        ll a; cin >> a;
        if(st.count(a) || a > n) can++;
        st.insert(a);
    }
    vector<ll> vec;
    for(auto x : st) vec.pb(x);
    while(true) {
        if(vec[cur-1] == cur +1) {
            cur++;
            continue;
        }else {
            if(can == 0) {
                if(vec.size()-cur+1 >= 2) {
                    vec.popb();vec.popb();
                    cur++;
                    continue;
                }
                cout << cur << endl;
                return 0;
            }
            can-=2;
            cur++;
        }
    }
    return 0;
}