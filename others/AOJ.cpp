#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using ld = long double;
using ull = unsigned long long;
using lll = __int128_t;

const long long inf = (1LL << 60);

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

template<class T> using vc   = vector<T>;
template<class T> using vvc  = vector<vc<T>>;
template<class T> using vvvc = vector<vvc<T>>;

using Graph = vector<vector<ll>>;
using Grid = vector<vector<char>>;


//====== ここから入出力系 ======

template<class A, class B>
ostream& operator<<(ostream& os, const pair<A,B>& p) {
    return os << "(" << p.first << "," << p.second << ")";
}

// 引数を空白区切りで出力
template<class T, class... Ts>
void out(const T& a, const Ts&... b) {
    cout << a;
    ((cout << ' ' << b), ...);
    cout << '\n';
}

// 引数を1行ずつ出力
template<class T, class... Ts>
void outendl(const T& a, const Ts&... b) {
    cout << a;
    ((cout << '\n' << b), ...);
    cout << '\n';
}

// 引数を順に読み込む
template<class... T>
void input(T&... a) {
    (cin >> ... >> a);
}


// __int128_t 出力
ostream& operator<<(ostream& os, lll x) {
    if (x == 0) return os << '0';

    if (x < 0) {
        os << '-';
        x = -x;
    }

    string s;
    while (x > 0) {
        int d = (int)(x % 10);
        s.push_back(char('0' + d));
        x /= 10;
    }

    reverse(s.begin(), s.end());
    return os << s;
}

// __int128_t 入力
istream& operator>>(istream& is, lll& x) {
    string s;
    is >> s;

    bool neg = false;
    int i = 0;

    if (!s.empty() && s[0] == '-') {
        neg = true;
        i = 1;
    }

    lll val = 0;

    for (; i < (int)s.size(); i++) {
        val = val * 10 + (s[i] - '0');
    }

    x = neg ? -val : val;

    return is;
}


//===== ここから便利系 ======

// a = min(a,b)
template<class T>
bool chmin(T& a, const T& b) {
    if (b < a) {
        a = b;
        return true;
    }
    return false;
}

// a = max(a,b)
template<class T>
bool chmax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}


//===== ここからデバッグ系 ======

template<class Array2D>
void For2(const Array2D& a) {
    for (const auto& row : a) {
        for (const auto& v : row) {
            cout << v << " ";
        }
        cout << '\n';
    }
}

template<class Array1D>
void For(const Array1D& x) {
    for (const auto& a : x) {
        cout << a << " ";
    }
    cout << '\n';
}


//解答コードここから
#ifndef ATCODER_LAZYSEGTREE_HPP
#define ATCODER_LAZYSEGTREE_HPP 1

#include <algorithm>
#include <cassert>
#include <functional>
#include <type_traits>
#include <vector>

namespace atcoder {

namespace internal {

// x 以上の最小の 2 のべき乗
constexpr unsigned int bit_ceil(unsigned int n) {
    unsigned int x = 1;
    while (x < n) x <<= 1;
    return x;
}

// 末尾に連続する 0 の個数
constexpr int countr_zero(unsigned int n) {
    int x = 0;
    while ((n & 1) == 0) {
        x++;
        n >>= 1;
    }
    return x;
}

}  // namespace internal


#if __cplusplus >= 201703L

template <class S,
          auto op,
          auto e,
          class F,
          auto mapping,
          auto composition,
          auto id>
struct lazy_segtree {
    static_assert(
        std::is_convertible_v<decltype(op), std::function<S(S, S)>>,
        "op must work as S(S, S)"
    );

    static_assert(
        std::is_convertible_v<decltype(e), std::function<S()>>,
        "e must work as S()"
    );

    static_assert(
        std::is_convertible_v<decltype(mapping), std::function<S(F, S)>>,
        "mapping must work as S(F, S)"
    );

    static_assert(
        std::is_convertible_v<decltype(composition), std::function<F(F, F)>>,
        "composition must work as F(F, F)"
    );

    static_assert(
        std::is_convertible_v<decltype(id), std::function<F()>>,
        "id must work as F()"
    );

#else

template <class S,
          S (*op)(S, S),
          S (*e)(),
          class F,
          S (*mapping)(F, S),
          F (*composition)(F, F),
          F (*id)()>
struct lazy_segtree {

#endif

public:

    lazy_segtree() : lazy_segtree(0) {}

    explicit lazy_segtree(int n)
        : lazy_segtree(std::vector<S>(n, e())) {}

    explicit lazy_segtree(const std::vector<S>& v)
        : _n(int(v.size())) {

        size = (int)internal::bit_ceil((unsigned int)(_n));

        log = internal::countr_zero((unsigned int)size);

        d = std::vector<S>(2 * size, e());

        lz = std::vector<F>(size, id());

        for (int i = 0; i < _n; i++) {
            d[size + i] = v[i];
        }

        for (int i = size - 1; i >= 1; i--) {
            update(i);
        }
    }


    void set(int p, S x) {

        assert(0 <= p && p < _n);

        p += size;

        for (int i = log; i >= 1; i--) {
            push(p >> i);
        }

        d[p] = x;

        for (int i = 1; i <= log; i++) {
            update(p >> i);
        }
    }


    S get(int p) {

        assert(0 <= p && p < _n);

        p += size;

        for (int i = log; i >= 1; i--) {
            push(p >> i);
        }

        return d[p];
    }


    S prod(int l, int r) {

        assert(0 <= l && l <= r && r <= _n);

        if (l == r) return e();

        l += size;
        r += size;

        for (int i = log; i >= 1; i--) {

            if (((l >> i) << i) != l) {
                push(l >> i);
            }

            if (((r >> i) << i) != r) {
                push((r - 1) >> i);
            }
        }


        S sml = e();
        S smr = e();

        while (l < r) {

            if (l & 1) {
                sml = op(sml, d[l++]);
            }

            if (r & 1) {
                smr = op(d[--r], smr);
            }

            l >>= 1;
            r >>= 1;
        }

        return op(sml, smr);
    }


    S all_prod() {
        return d[1];
    }


    void apply(int p, F f) {

        assert(0 <= p && p < _n);

        p += size;

        for (int i = log; i >= 1; i--) {
            push(p >> i);
        }

        d[p] = mapping(f, d[p]);

        for (int i = 1; i <= log; i++) {
            update(p >> i);
        }
    }


    void apply(int l, int r, F f) {

        assert(0 <= l && l <= r && r <= _n);

        if (l == r) return;

        l += size;
        r += size;

        for (int i = log; i >= 1; i--) {

            if (((l >> i) << i) != l) {
                push(l >> i);
            }

            if (((r >> i) << i) != r) {
                push((r - 1) >> i);
            }
        }


        {
            int l2 = l;
            int r2 = r;

            while (l < r) {

                if (l & 1) {
                    all_apply(l++, f);
                }

                if (r & 1) {
                    all_apply(--r, f);
                }

                l >>= 1;
                r >>= 1;
            }

            l = l2;
            r = r2;
        }


        for (int i = 1; i <= log; i++) {

            if (((l >> i) << i) != l) {
                update(l >> i);
            }

            if (((r >> i) << i) != r) {
                update((r - 1) >> i);
            }
        }
    }


    template <bool (*g)(S)>
    int max_right(int l) {
        return max_right(l, [](S x) {
            return g(x);
        });
    }


    template <class G>
    int max_right(int l, G g) {

        assert(0 <= l && l <= _n);
        assert(g(e()));

        if (l == _n) {
            return _n;
        }

        l += size;

        for (int i = log; i >= 1; i--) {
            push(l >> i);
        }

        S sm = e();

        do {

            while (l % 2 == 0) {
                l >>= 1;
            }

            if (!g(op(sm, d[l]))) {

                while (l < size) {

                    push(l);

                    l = 2 * l;

                    if (g(op(sm, d[l]))) {

                        sm = op(sm, d[l]);

                        l++;
                    }
                }

                return l - size;
            }

            sm = op(sm, d[l]);

            l++;

        } while ((l & -l) != l);

        return _n;
    }


    template <bool (*g)(S)>
    int min_left(int r) {
        return min_left(r, [](S x) {
            return g(x);
        });
    }


    template <class G>
    int min_left(int r, G g) {

        assert(0 <= r && r <= _n);
        assert(g(e()));

        if (r == 0) {
            return 0;
        }

        r += size;

        for (int i = log; i >= 1; i--) {
            push((r - 1) >> i);
        }

        S sm = e();

        do {

            r--;

            while (r > 1 && (r % 2)) {
                r >>= 1;
            }

            if (!g(op(d[r], sm))) {

                while (r < size) {

                    push(r);

                    r = 2 * r + 1;

                    if (g(op(d[r], sm))) {

                        sm = op(d[r], sm);

                        r--;
                    }
                }

                return r + 1 - size;
            }

            sm = op(d[r], sm);

        } while ((r & -r) != r);

        return 0;
    }


private:

    int _n;
    int size;
    int log;

    std::vector<S> d;
    std::vector<F> lz;


    void update(int k) {

        d[k] = op(
            d[2 * k],
            d[2 * k + 1]
        );
    }


    void all_apply(int k, F f) {

        d[k] = mapping(
            f,
            d[k]
        );

        if (k < size) {

            lz[k] = composition(
                f,
                lz[k]
            );
        }
    }


    void push(int k) {

        all_apply(
            2 * k,
            lz[k]
        );

        all_apply(
            2 * k + 1,
            lz[k]
        );

        lz[k] = id();
    }
};

}  // namespace atcoder

#endif
/* From chanini library*/
//区間加算 → 区間和 + 区間最小値 + 区間最大値

// S : セグ木が持つ値
struct S {
    ll sum;   // 区間和
    ll mn;    // 区間最小値
    ll mx;    // 区間最大値
    ll size;  // 区間の要素数
};

// F : 遅延させる更新 → +x
using F = long long;

// 2区間を結合
S op(S a, S b) {
    return {
        a.sum + b.sum,
        min(a.mn, b.mn),
        max(a.mx, b.mx),
        a.size + b.size
    };
}

// S側の単位元
S e() {
    return {
        0,      // sum
        inf,    // mn
        -inf,   // mx
        0       // size
    };
}

// 更新 f をデータ x に適用
// 区間全体に +f
S mapping(F f, S x) {

    // 空区間なら何もしない
    if(x.size == 0) return x;

    return {
        x.sum + f * x.size,
        x.mn + f,
        x.mx + f,
        x.size
    };
}

// 更新を合成
// +g のあとに +f → +(f+g)
ll composition(F f, F g) {return f + g;}

// F側の単位元 +0
ll id(){return 0;}

/*
    【使い方】
    【できること】
    ・区間加算
    ・区間和
    ・区間最小値
    ・区間最大値
    ・一点取得
    ・一点変更


   // [l,r) に +x
    seg.apply(l, r, x);

   // p に +x
    seg.apply(p, x);

   // [l,r) の情報取得
    S res = seg.prod(l, r);
   res.sum;   // 区間和
   res.mn;    // 区間最小値
   res.mx;    // 区間最大値
   res.size;  // 区間サイズ

   // 全区間の情報
    S res = seg.all_prod();

   // p 番目の情報
    S res = seg.get(p);

   // p 番目を x に変更
    seg.set(p, {x, x, x, 1});
*/
using namespace atcoder;
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    
    return 0;
}