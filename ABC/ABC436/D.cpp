// ==================== MAIN（ここを編集） ====================
#ifdef __INTELLISENSE__
#define INCLUDED_MAIN 1   // IntelliSenseにはmain側を見せる
#endif

#if !__INCLUDE_LEVEL__
#include __FILE__


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // ここに解答コード
    cout << a << endl;  // a未定義ならすぐ赤波線が出る
    return 0;
}

// ==================== LIB（ここから下はライブラリ） ====================
#else

#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;

using ll = long long;
// 例：型
using Graphchar = vector<vector<char>>;
using Graphll   = vector<vector<ll>>;

// 例：マクロ
#define rep(i,a,b) for(int i=(a); i<(b); i++)
#define pb push_back
#define fi first
#define se second

// ここに関数/テンプレ/便利ツールを全部置く（下に置いてOK）
// template<class T> ... などもここでOK


#endif
