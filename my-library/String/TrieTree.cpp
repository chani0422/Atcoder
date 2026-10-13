

#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
template<int char_size,int base>
struct Trie{
    struct Node {
        array<int, char_size> next;
        
        //この頂点を通過する文字列の個数
        int common = 0;
        
        //この頂点で終了する文字列の個数
        int terminal = 0;

        Node() {
            next.fill(-1);
        }
    };

    vector<Node> nodes;
    Trie() {
        nodes.emplace_back();
    }

    int char_id(char c) const {
        int id = c - base;
        assert(0 <= id && id < char_size);
        return id;
    }


    // 文字列 word をTrieに1個追加し、
    // wordの末端に対応する頂点番号を返す。
    // 同じ文字列を複数回追加した場合も別々に数える。
    // 計算量：O(|word|)
        int insert(const string& word) {
        int node_id = 0;
        ++nodes[node_id].common;

        for(char ch : word) {
            int c = char_id(ch);
            int next_id = nodes[node_id].next[c];

            if(next_id == -1) {
                next_id = (int)nodes.size();
                nodes[node_id].next[c] = next_id;
                nodes.emplace_back();
            }
            node_id = next_id;
            ++nodes[node_id].common;
        }
        ++nodes[node_id].terminal;
        return node_id;
    }
    //wordに対応する頂点番号を渡す。
    //存在しなければ-1
    int find_node(const string& word) const {
        int node_id = 0;
        for(char ch : word) {
            int c = char_id(ch);
            int next_id = nodes[node_id].next[c];
            if(next_id == -1) return -1;
            node_id = next_id;
        }
        return node_id;
    }

    //wordが登録されているか
    bool search(const string& word) const {
        int node_id = find_node(word);
        return node_id != -1 && nodes[node_id].terminal > 0;
    }

    //prefixから始まる文字列が存在するか
    bool starts_with(const string& prefix) const {
        int node_id = find_node(prefix);
        return node_id != -1 && nodes[node_id].common > 0;
    }
    
    //wordが登録された個数
    int count(const string& word) const {
        int node_id = find_node(word);
        if(node_id == -1) return 0;
        return nodes[node_id].terminal;
    }

    //prefixから始まる文字列の個数
    int prefix_count(const string& prefix) const {
        int node_id = find_node(prefix);
        if(node_id == -1) return 0;
        return nodes[node_id].common;
    }

    //登録した文字列の総数
    int count() const {
        return nodes[0].common;
    }

    //Trieの頂点数
    int size() const {
        return (int)nodes.size();
    }

    // 登録済みの word と、別の登録文字列との
    // 最長共通接頭辞（LCP）の最大長を返す。
    // wordがすでにTrieに登録されていることを前提とする。
    // 同じ文字列が複数回登録されている場合は、それぞれ別の文字列として扱う。
    // 計算量：O(|word|)
    int max_lcp(const string& word) const {
        int node_id = 0;
        int answer = 0;

        for (int i = 0; i < (int)word.size(); i++) {
            int c = char_id(word[i]);
            int next_id = nodes[node_id].next[c];

            if (next_id == -1) break;
            if (nodes[next_id].common < 2) break;

            node_id = next_id;
            answer = i + 1;
        }

        return answer;
    }
    // 登録されている word を1個削除する。
    // 削除に成功した場合はtrue、wordが未登録ならfalseを返す。
    // 不要になった頂点自体は削除せず、commonとterminalだけを減らす。
    // 同じ文字列が複数登録されている場合は、そのうち1個だけ削除する。
    // 計算量：O(|word|)
    bool erase(const string& word) {
        if (!search(word)) return false;

        int node_id = 0;
        --nodes[node_id].common;

        for (char ch : word) {
            int c = char_id(ch);
            node_id = nodes[node_id].next[c];
            --nodes[node_id].common;
        }

        --nodes[node_id].terminal;
        return true;
    }

    // Trieが使用する頂点領域をあらかじめ確保する。
    // max_nodesには「全文字列の長さの合計 + 1」を指定するとよい。
    // 領域を確保するだけなので、Trieの頂点数自体は増加しない。
    // 大量の頂点追加によるvectorの再確保を減らすために使用する。
    void reserve(int max_nodes) {
        nodes.reserve(max_nodes);
    }

    // 根からwordに対応する頂点までの経路を、頂点番号の配列で返す。
    // 返される配列には根（頂点0）も含まれるため、要素数は |word|+1。
    // wordに対応する経路が存在しない場合は空の配列を返す。
    // 計算量：O(|word|)
    // 追加メモリ：O(|word|)
    vector<int> get_path(const string& word) const {
        vector<int> path;
        int node_id = 0;
        path.push_back(0);

        for (char ch : word) {
            int c = char_id(ch);
            int next_id = nodes[node_id].next[c];

            if (next_id == -1) {
                return {};
            }

            node_id = next_id;
            path.push_back(node_id);
        }

        return path;
    }

};

/*
    Trie<char_size, base>

    insert(s)          : 文字列sを追加
    search(s)          : 文字列sが登録されているか
    starts_with(s)     : sを接頭辞とする文字列が存在するか
    count(s)           : 文字列sが登録された個数
    prefix_count(s)    : sを接頭辞とする文字列の個数
    count()            : 登録した文字列の総数
    size()             : Trieの頂点数

    計算量：
    挿入・検索ともに O(|s|)
*/
//問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    Trie<26, 'A'> trie;
    trie.reserve(500001);

    trie.insert("FIRE");
    trie.insert("FIRE");
    trie.insert("FIREMAN");
    trie.insert("FIREARM");
    trie.insert("FIREWORK");
    trie.insert("ALGO");

    cout << trie.search("FIRE") << '\n';          // 1
    cout << trie.search("FI") << '\n';            // 0
    cout << trie.starts_with("FI") << '\n';       // 1
    cout << trie.count("FIRE") << '\n';           // 2
    cout << trie.prefix_count("FIRE") << '\n';    // 5
    cout << trie.prefix_count("FI") << '\n';      // 5
    cout << trie.count() << '\n';                 // 6
    cout << trie.size() << '\n';                  // 頂点数

    auto path = trie.get_path("FIRE");

    cout << "\n";
    for (int v : path) {
        cout << trie.nodes[v].common << " ";
    }
    cout << "\n";
    for (int v : path) {
    cout << v << " ";
    }
    return 0;
}
//解答コードここまで

#else

// === LIB_BEGIN ===
#include <bits/stdc++.h>
#include <atcoder/all>
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
