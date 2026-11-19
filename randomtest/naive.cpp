
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
    int N;
    ll K;
    cin >> N >> K;

    vector<ll> A(N);
    for (auto &x : A) cin >> x;

    ll ans = 0;

    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (A[i] + A[j] == K) {
                ans++;
            }
        }
    }

    cout << ans << '\n';
}