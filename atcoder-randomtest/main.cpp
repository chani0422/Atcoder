#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// Sample task: maximum sum of a non-empty contiguous subarray.
// Replace this file with your actual solution.
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n; cin >> n;
    vector<ll> a(n);
    for (auto &x : a) cin >> x;
    ll cur = a[0], ans = a[0];
    for (int i = 1; i < n; ++i) {
        cur = max(a[i], cur + a[i]);
        ans = max(ans, cur);
    }
    cout << ans << '\n';
}
