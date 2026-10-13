#include<bits/stdc++.h>
using namespace std;
using ll = long long;

//AWC0127 Eの時、ダブルハッシュに進化しました。
struct RollingHash {
    static constexpr ll MOD1 = 1000000007;
    static constexpr ll MOD2 = 1000000009;
    static constexpr ll BASE = 911382323;
    vector<ll> hash1,hash2;
    vector<ll> power1,power2;
    RollingHash(const string& s) {
        ll n = s.size();
        hash1.assign(n+1,0);
        hash2.assign(n+1,0);
        power1.assign(n+1,1);
        power2.assign(n+1,1);

        for(int i = 0; i < n; i++) {
            ll x = static_cast<unsigned char>(s[i]) + 1;
            hash1[i + 1] = (hash1[i] * BASE + x) % MOD1;
            hash2[i + 1] = (hash2[i] * BASE + x) % MOD2;
            power1[i + 1] = power1[i] * BASE % MOD1;
            power2[i + 1] = power2[i] * BASE % MOD2;

        }
    }

    pair<ll,ll> get(int l, int r) const {
        ll result1 = hash1[r] - hash1[l] * power1[r-l] % MOD1;
        ll result2 = hash2[r] - hash2[l] * power2[r-l] % MOD2;
        if(result1 < 0) result1 += MOD1;
        if(result2 < 0) result2 += MOD2;
        return {result1,result2};
    }
};


//from chanini library
struct RollingHash {
    using ll = long long;

    static constexpr ll MOD = 1000000007;
    static constexpr ll BASE = 911382323;

    vector<ll> hash;
    vector<ll> power;

    RollingHash(const string& s) {
        int n = s.size();

        hash.assign(n + 1, 0);
        power.assign(n + 1, 1);

        for (int i = 0; i < n; i++) {
            // +1して文字コード0との区別をつける
            ll x = static_cast<unsigned char>(s[i]) + 1;

            hash[i + 1] = (hash[i] * BASE + x) % MOD;
            power[i + 1] = power[i] * BASE % MOD;
        }
    }

    // s[l, r) のハッシュ値
    ll get(int l, int r) const {
        ll result =
            hash[r] - hash[l] * power[r - l] % MOD;

        if (result < 0) result += MOD;

        return result;
    }
};

//使い方
int main() {
    string s = "abcabc";
    RollingHash rh(s);
    rh.get(0,3);
    
    return 0;
}