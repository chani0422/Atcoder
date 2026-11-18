#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// Simple O(N^2) reference solution to the sample task.
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n; cin >> n;
    vector<ll> a(n);
    for (auto &x : a) cin >> x;
    ll ans = LLONG_MIN;
    for (int l = 0; l < n; ++l) {
        ll s = 0;
        for (int r = l; r < n; ++r) {
            s += a[r];
            ans = max(ans, s);
        }
    }
    cout << ans << '\n';
}
