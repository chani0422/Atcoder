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
    string s; cin >> s;
    int n = s.size();
    int cnt = 0;
    string ss = "";
    rep(i,0,n) {
        if(s[i] == '0') {
            cnt++;
            continue;
        }else {
            ss += s[i];
        }
    }
    sort(nall(ss));

    string m = "";
    m += ss[0];
    rep(i,0,cnt) m += "0";
    rep(i,1,ss.size()) m += ss[i];
    cout << m << endl;
    return 0;
}