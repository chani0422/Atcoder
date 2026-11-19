
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    ll K;
    cin >> N >> K;

    vector<ll> A(N);
    for (auto &x : A) cin >> x;

    sort(A.begin(), A.end());

    int l = 0, r = N - 1;
    ll ans = 0;

    while (l < r) {
        ll sum = A[l] + A[r];

    if (sum == K) {
        if (A[l] == A[r]) {
            ll cnt = r - l + 1;
            ans += cnt * (cnt - 1) / 2;
            break;
        }

        ll cntL = 0, cntR = 0;
        ll x = A[l], y = A[r];

        while (l <= r && A[l] == x) {
            cntL++;
            l++;
        }
        while (l <= r && A[r] == y) {
            cntR++;
            r--;
        }

        ans += cntL * cntR;
        } else if (sum < K) {
            l++;
        } else {
            r--;
        }
    }

    cout << ans << '\n';
}