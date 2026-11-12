#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;
#define rep(i,a,b) for(int i=a;i<(b);i++)
#define rrep(i,a,b) for(int i=a;i<=(b);i++)
using ll = long long;
using ull = unsigned long long;
using mint = modint998244353;

// ll n,m;
// vector<ll> a;
// bool solve(ll middle){
//   ll total = 0;
//   rep(i,0,n){
//     total+=min(a[i],middle);
//   }
//   if(total <= m){
//     return true;
//   }
//   else if(total > m){
//     return false;
//   }
// }
int main() {
  ios_base::sync_with_stdio(false);cin.tie(nullptr);
  ll n,m;cin >> n >> m;
  vector<ll> a(n);
  ll sum = 0;
  rep(i,0,n){
    cin >> a[i];
    sum+=a[i];
  }
  if(sum <= m){
    cout << "infinite" << endl;
  }
  else{
    ll right = 1000000000000000000;
    ll left = 0;
    while(right-left > 1){
      ll mid = (right+left)/2;
      ll total = 0;
      rep(i,0,n){
        total+=min(a[i],mid);
      }
      if(total <= mid){
        left = mid;
      }
      else if(total > mid){
        right = mid;
      }

    }
    cout << left << endl;
  }
    return 0;
}