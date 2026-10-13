#include <bits/stdc++.h>
using namespace std;
const long long inf = (1LL<<60);
#define rep(i,a,b) for(ll i=(a); i<(b); i++)
#define rrep(i,a,b) for(ll i=(a); i>=(b); i--)
#define pb push_back
#define popb() pop_back()
#define fi first
#define se second
#define yn(x) do { if(x) cout << "Yes\n"; else cout << "No\n"; } while(0)
#define nall(a) (a).begin(),(a).end()
#define rall(a) (a).rbegin(),(a).rend()
#define MIN(x) *min_element(nall(x))
#define MAX(x) *max_element(nall(x))

using ll = long long;
using ld = long double;
using ull = unsigned long long;
using lll = __int128_t;


using Graph= vector<vector<ll>>;
using Grid = vector<vector<char>>;



//===== ここから便利系 =====
//ans = min(ans,x)をやる。ついでに戻り値もtrue,falseで返す。
template <class T>
bool chmin(T& a, const T& b) {
    if (b < a) { a = b; return true; }
    return false;
}
//ans = max(ans,x)をやる。ついでに戻り値もtrue,falseで返す。
template <class T>
bool chmax(T& a, const T& b) {
    if (a < b) { a = b; return true; }
    return false;
}

//===== ここからデバッグ系 =====
template<class Array2D>
void For2(const Array2D& a) {
    for (const auto& row : a) {
        for (const auto& v : row) cout << v << " ";
        cout << "\n";
    }
}
template <class Array1D>
void For(const Array1D& x) {
    for (const auto& a : x) cout << a << " ";
    cout << '\n';
}


int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    while(true) {
        ll a,b,c,d;
        cin >> a >> b >> c >> d;
        if(a==0&&b==0&&c==0&&d==0) break;
        vector<ll> A = {a,b,c,d};
        bool finish = false;
        ll cnt = 0;
        while(true) {
            if(cnt == 3) {
                ll ans = *min_element(nall(A));
                rep(i,0,4) {
                    if(A[i] <= 100) ans = A[i];
                }
                cout << ans << endl;
                finish = true;
                break;
            }
            for(ll &x : A) if(x == 0) {x = 1000;cnt++;}
            ll mn = *min_element(nall(A));
            bool skip = false;
                rep(i,0,4) {
                    if(skip == false && mn == A[i]) {
                        skip = true;
                        continue;
                    }
                    A[i] -= mn;
                    if(A[i] <= 0) {
                        A[i] = 1000;
                        cnt++;
                    }
                }
            }
    }
    return 0;
}