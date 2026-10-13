
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
//from chanini library
//自前のロリハ！！
struct RollingHash {
    static constexpr ll MOD = 1000000007;
    const ll BASE = 911382323;
    vector<ll> hash;
    vector<ll> power;
    RollingHash(const string& s) {
        ll n = s.size();
        hash.assign(n+1,0);
        power.assign(n+1,1);

        for(int i = 0; i < n; i++) {
            ll x = static_cast<unsigned char>(s[i]) + 1;
            hash[i + 1] = (hash[i] * BASE + x) % MOD;
            power[i + 1] = power[i] * BASE % MOD;

        }
    }

    ll get(int l, int r) const {
        ll result = hash[r] - hash[l] * power[r-l] % MOD;
        if(result < 0) result += MOD;
        return result;
    }
};

//sとrevをl1,l2から最大limit文字以内での一致する最長の長さを返す
ll nibutan(RollingHash &rh1,ll l1,RollingHash &rh2,ll l2,ll limit) {
    ll cnt = 0;
    ll mx = limit + 1;

    while(mx - cnt > 1) {
        ll mid = (cnt+mx)/2;

        if(rh1.get(l1,l1+mid) == rh2.get(l2,l2+mid)) {
            cnt = mid;
        }else {
            mx = mid;
        }
    }
    return cnt;
}

//D問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    string s; cin >> s;
    string rev = s;
    reverse(nall(rev));
    ll n = s.size();
    RollingHash rh1(s);
    RollingHash rh2(rev);

    ll ans = 0;
    //which=0は偶数長、which=1は奇数長
    rep(which,0,2) {
        rep(chushin,which,n) {
            ll r = chushin + which;
            ll l = n - chushin;
            ll limit;
            if(which == 0) {//偶数
                limit = min(chushin,n-chushin);
            }else {//奇数・・・一つ右から始める
                limit = min(chushin,n-chushin-1);
            }

            ll same = nibutan(rh1,r,rh2,l,limit);

            //不一致があれば一回はOKだからそれより後で次不一致になるまで調べる（日本語むずい）
            if(same < limit) {
                ll next = same + 1;
                same = next + nibutan(rh1,r+next,rh2,l+next,limit-next);
            }
            //奇数長の場合のみ真ん中の文字もカウントする
            ans += (same +((which) ? 0:1));
        }
    }

    cout << ans << endl;

    return 0;
}
//解答コードここまで

#else

// === LIB_BEGIN ===
#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;

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

template<class T> using vc   = vector<T>;
template<class T> using vvc  = vector<vc<T>>;
template<class T> using vvvc = vector<vvc<T>>;

using Graph= vector<vector<ll>>;
using Grid = vector<vector<char>>;


//====== ここから入出力系 ======
template<class A, class B>
ostream& operator<<(ostream& os, const pair<A,B>& p) {
    return os << "(" << p.first << "," << p.second << ")";
}

//引数を空白区切りで出力
template<class T, class... Ts>
void out(const T& a, const Ts&... b){
    cout << a;
    ((cout << ' ' << b), ...);
    cout << '\n';
}
//引数を1行づつ出力
template<class T, class... Ts>
void outendl(const T& a, const Ts&... b){
    cout << a;
    ((cout << endl << b), ...); 
    cout << '\n';
}
//引数を順に読み込む
template <class... T>
void input(T &...a) {
	(cin >> ... >> a);
}

// __int128_tでも入出力できるようにする
// cout 用
ostream& operator<<(ostream& os, lll x) {
    if (x == 0) return os << '0';
    if (x < 0) { os << '-'; x = -x; } // 通常ケース

    string s;
    while (x > 0) {
        int d = (int)(x % 10);
        s.push_back(char('0' + d));
        x /= 10;
    }
    reverse(s.begin(), s.end());
    return os << s;
}

// cin 用
istream& operator>>(istream& is, lll& x) {
    string s;
    is >> s;
    bool neg = false;
    int i = 0;
    if (!s.empty() && s[0] == '-') { neg = true; i = 1; }

    lll val = 0;
    for (; i < (int)s.size(); i++) {
        val = val * 10 + (s[i] - '0');
    }
    x = neg ? -val : val;
    return is;
}

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

// === LIB_END ===

#endif
