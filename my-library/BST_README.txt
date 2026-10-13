ある。結論からいうと、

前の「動的中央値」のような問題 → PBDS
vector のように途中挿入・削除したい → KACTL の Implicit Treap
区間反転・区間更新まで欲しい → Nyaan の Splay Tree

が良いです。

目的	おすすめ	特徴
値順に管理・k番目・順位	GNU PBDS	赤黒木。実装不要で安定
位置による挿入・削除	KACTL Treap	約65行、stress-tested。自作ライブラリの土台に最適
区間和・区間更新・反転	Nyaan Lazy Reversible Splay Tree	高機能でLibrary Checkerによる検証あり
仕組みを学びながら作る	cp-algorithms Treap	通常TreapとImplicit Treapの解説が詳しい

特に、前に扱っていた「点数を更新しながら中央値を求める」用途なら、TreapVector よりPBDSの方が素直です。

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace __gnu_pbds;

template<class T>
using ordered_set =
    tree<
        T,
        null_type,
        less<T>,
        rb_tree_tag,
        tree_order_statistics_node_update
    >;

重複を扱うなら、値と固有IDのペアにします。

using P = pair<long long, int>;
ordered_set<P> S;

S.insert({100, 0});
S.insert({100, 1});
S.insert({80, 2});

// 0-indexedでk番目
auto [score, id] = *S.find_by_order(1);

// {90, -∞} より小さい要素数
int cnt = S.order_of_key({90, INT_MIN});

// 特定要素を削除
S.erase({100, 0});

ネットでよく見る、

tree<int, null_type, less_equal<int>, ...>

による「PBDS版multiset」は比較関数の条件を破るので避け、pair<値, 固有ID> にするのが安全です。AtCoderの現在のC++23環境もGCC 15.2.0なので、PBDSを利用できます。AtCoder公式の言語環境情報

一方、TreapVector を本格的に作り直したいなら、KACTL版を土台にして次を追加するのがおすすめです。

operator[]
insert(pos, value)
erase(pos)
push_back / pop_back
split / merge
reverse(l, r)
区間集約
ノードプール
vector と比較するランダムテスト

つまり、赤黒木をゼロから書くより、まずKACTLの短いTreapを自分用のクラスに仕上げるのが一番現実的です。

君がネットにあるコードを色々調べて、いい感じの平衡二分探索木を作って
できるだけ多機能でお願い

完成しました。乱択Treap方式の、C++17対応・単一ヘッダライブラリです。

主な機能：

TreapSet / TreapMultiSet
挿入・削除・重複管理
kth
order_of_key
lower_bound / upper_bound
前後要素・中央値
TreapMap
キー順map＋順位・k番目
TreapVector
途中挿入・削除
区間反転・回転・移動・交換・ソート
NumericTreapVector
区間加算・代入・乗算・一次変換
区間和・最小・最大
任意モノイド、非可換演算、max_right / min_left
ノードプール・削除領域再利用・O(N) 一括構築

ファイル：

balanced_treap.hpp — 本体
README_balanced_treap.md — 日本語API説明
balanced_treap_example.cpp — 使用例
test_balanced_treap.cpp — テストコード

KACTL、suisen-cp、Nyaan、cp-algorithms、GNU PBDSを比較し、APIと実装を新しく組み直しています。

以下をすべて通過済みです。

STLとの合計24万回以上のランダム差分テスト
20万要素の構築・削除・再挿入
AddressSanitizer / UBSan
_GLIBCXX_DEBUG
C++17・C++23の警告付きビルド

なお乱択Treapなので、主な計算量は最悪保証ではなく期待 O(log N) です。まずREADMEと使用例を見ると、そのままテンプレートへ追加できます。

balanced_treap.hpp
コード
README_balanced_treap.md
ドキュメント
balanced_treap_example.cpp
コード
他 1 件

ライブラリ
/
README_balanced_treap.md
多機能平衡二分探索木ライブラリ

balanced_treap.hpp は、AtCoderなどの競技プログラミング向けに作ったC++17の単一ヘッダライブラリです。

平衡二分探索木には、次の2つの異なる使い方があります。このライブラリでは、それらを別コンテナとして提供します。

種類	並び順	主な用途
TreapSet / TreapMultiSet	値の大小	順位、k番目、中央値、前後の値
TreapMap	キーの大小	キーと値の対応、k番目のキー
TreapVector 系	配列内の位置	途中挿入・削除、区間反転、区間更新

両方とも乱択Treapです。主な操作は期待 O(log N)、列をまとめて構築する処理は O(N) です。

導入

balanced_treap.hpp を提出コードと同じ場所に置きます。

#include "balanced_treap.hpp"

すべてのクラスは cp 名前空間にあります。

1. TreapSet / TreapMultiSet
cp::TreapSet<long long> st;
cp::TreapMultiSet<long long> mst;
操作一覧
操作	意味	計算量
insert(x)	挿入。Setでは新規挿入できたか返す	期待 O(log N)
erase_one(x)	x を1個削除	期待 O(log N)
erase_all(x) / erase(x)	x をすべて削除し、削除数を返す	期待 O(log N)
contains(x)	x が存在するか	期待 O(log N)
count(x)	x の個数	期待 O(log N)
kth(k)	0-indexedでk番目の値	期待 O(log N)
find_by_order(k)	k番目。範囲外なら nullopt	期待 O(log N)
order_of_key(x)	x 未満の要素数	期待 O(log N)
order_of_key_upper(x)	x 以下の要素数	期待 O(log N)
count_range(l,r)	[l,r) に入る要素数	期待 O(log N)
lower_bound(x)	x 以上の最小値	期待 O(log N)
upper_bound(x)	x より大きい最小値	期待 O(log N)
predecessor(x)	x 未満の最大値	期待 O(log N)
successor(x)	x より大きい最小値	期待 O(log N)
floor(x) / ceiling(x)	x 以下の最大・x 以上の最小	期待 O(log N)
median_lower/upper()	下側・上側中央値	期待 O(log N)
min() / max()	最小値・最大値	期待 O(log N)
pop_min() / pop_max()	最小・最大を取り出して削除	期待 O(log N)
extract_by_order(k)	k番目を取り出して削除	期待 O(log N)
to_vector()	ソート済みvectorへ変換	O(N)
distinct_to_vector()	異なる値だけをvectorへ変換	O(N)
assign(first,last)	入力をソートして一括構築	O(N log N)
assign_sorted(first,last)	ソート済み入力から一括構築	O(N)
size() / empty()	要素数・空判定	O(1)
distinct_size()	異なる値の個数	O(1)
validate()	内部構造を検査するデバッグ用関数	O(N)
重複ありの中央値
cp::TreapMultiSet<long long> s;
for (long long x : a) s.insert(x);

long long median = s.kth((s.size() - 1) / 2);

値を個人ごとに更新するなら、pair<値, 固有ID> を使うと安全です。

cp::TreapSet<pair<long long, int>> s;
s.insert({score[i], i});
s.erase_one({score[i], i});
2. TreapMap

キー順のmapに、k番目と順位を追加したコンテナです。

cp::TreapMap<int, string> mp;

mp.insert(10, "ten");
mp.insert_or_assign(20, "twenty");
mp.modify(20, [](string& s) { s += "!"; });

auto value = mp.get(20);       // optional<string>
auto [key, mapped] = mp.kth(0); // キー順で0番目
size_t rank = mp.order_of_key(20);

主な操作は insert、insert_or_assign、erase、contains、get、at、modify、kth、find_by_order、order_of_key、lower_bound、upper_bound、predecessor、successor、to_vector です。

内部集約を壊さないよう、mapped valueは参照ではなく値として返します。変更には insert_or_assign または modify を使用します。

3. TreapVector
cp::TreapVector<long long> a{1, 2, 3, 4, 5};
配列操作
操作	意味	計算量
get(i) / at(i) / a[i]	i 番目を値として取得	期待 O(log N)
set(i,x)	a[i]=x	期待 O(log N)
modify(i,f)	a[i] に関数を適用	期待 O(log N)
insert(i,x)	i 番目の直前へ挿入	期待 O(log N)
erase(i)	i 番目を削除して返す	期待 O(log N)
push_front/back(x)	先頭・末尾へ挿入	期待 O(log N)
pop_front/back()	先頭・末尾を削除して返す	期待 O(log N)
insert_range(i,v)	複数要素をまとめて挿入	期待 O(K+log N)
erase_range(l,r)	[l,r) を削除してvectorで返す	期待 O(K+log N)
replace_range(l,r,v)	[l,r) を別の列で置換	期待 O(K+log N)
slice(l,r)	[l,r) をvectorで取得	期待 O(K+log N)
reverse(l,r)	[l,r) を反転	期待 O(log N)
rotate(l,m,r)	[l,m)[m,r) を [m,r)[l,m) へ	期待 O(log N)
move_range(l,r,pos)	[l,r) を元の境界 pos の直前へ移動	期待 O(log N)
swap_ranges(...)	重ならない2区間を交換。長さ違いも可	期待 O(log N)
sort_range(l,r)	指定区間をソート	O(K log K)
sort()	全体をソート	O(N log N)
to_vector()	現在の列をvectorへ変換	O(N)

operator[] は値を返します。参照を直接書き換えると部分木情報が更新されないため、変更には必ず set または modify を使います。

a.set(3, 100);
a.modify(3, [](long long& x) { x += 5; });
整列済み列に対する二分探索
a.sort();
size_t l = a.lower_bound_sorted(x);
size_t r = a.upper_bound_sorted(x);
bool exists = a.binary_search_sorted(x);

_sorted が付く関数は、列が整列済みの場合だけ正しい結果になります。途中へ自由に挿入した直後などには使用できません。

4. NumericTreapVector

整数などの数値列に対し、配列操作に加えて区間更新と区間集約を扱えます。

cp::NumericTreapVector<long long> a{1, 2, 3, 4, 5};

a.range_add(1, 4, 10);       // x <- x + 10
a.range_multiply(0, 3, -1);  // x <- -x
a.range_assign(2, 5, 7);     // x <- 7
a.range_affine(0, 5, 2, 3);  // x <- 2*x + 3

long long sum = a.range_sum(0, 5);
long long mn  = a.range_min(0, 5);
long long mx  = a.range_max(0, 5);

各操作は期待 O(log N) です。range_affine(l,r,mul,add) は区間内の全要素を x = mul*x + add に変換します。mul が負の場合の最小値・最大値の入れ替えにも対応しています。

計算途中のオーバーフローは検出しません。

5. AffineSumTreapVector

最小値・最大値が不要で、和だけ取得したい場合に使います。比較演算を必要としないため、atcoder::modint のような型にも適しています。

cp::AffineSumTreapVector<mint> a(values.begin(), values.end());
a.range_affine(l, r, b, c);
mint answer = a.range_sum(l, r);
6. 任意モノイドの区間集約

FoldableTreapVector<T,Op> または ImplicitTreap<T,Policy> を使用します。前向き集約と逆向き集約を両方保持するため、文字列連結や関数合成のような非可換演算でも区間反転できます。

struct Concat {
    string operator()(const string& a, const string& b) const {
        return a + b;
    }
};

using Policy = cp::FoldSequencePolicy<string, Concat>;
cp::ImplicitTreap<string, Policy> s(
    vector<string>{"A", "B", "C"},
    Policy{"", Concat{}}
);

s.reverse(0, 3);
cout << s.range_query(0, 3); // "CBA"

完全に独自の遅延作用を追加する場合、Policyはヘッダ内に記載された次のインターフェースを実装します。

using aggregate_type = ...;
using tag_type = ...;

aggregate_type identity() const;
aggregate_type make_aggregate(const T&) const;
aggregate_type combine(const aggregate_type&, const aggregate_type&) const;
void apply_value(T&, const tag_type&) const;
void apply_aggregate(aggregate_type&, const tag_type&, size_t length) const;
tag_type compose(const tag_type& newer, const tag_type& older) const;

compose(newer,older) は「older を適用し、その後 newer を適用する」作用を返します。区間作用は要素ごとの作用で、区間反転と適用順序を交換できる必要があります。

7. max_right / min_left

ACLのセグメント木と同様の境界探索を利用できます。

cp::AffineSumTreapVector<long long> a{1, 2, 3, 4, 5};

size_t r = a.max_right(0, [](const cp::SumAggregate<long long>& x) {
    return x.sum <= 6;
}); // r=3: 1+2+3 <= 6

述語は単調で、単位元に対して true でなければなりません。計算量は期待 O(log N) です。

実装上の特徴
ポインタではなく整数インデックスを使用
削除したノード領域を再利用
SplitMix64による64-bit優先度
デフォルトseed固定でデバッグ結果を再現可能
Cartesian Treeとしての一括構築は O(N)
順方向・逆方向の集約を保持
単一ヘッダ、C++17対応、外部依存なし
コピー・ムーブ可能
validate() / validate_structure() で内部構造を検査可能

乱択Treapなので計算量は期待値です。赤黒木のような最悪計算量 O(log N) の保証はありません。通常の競技プログラミング用途では十分高速ですが、敵対的に優先度を推測される状況ではseedを変更できます。

cp::TreapMultiSet<int> s(std::less<int>{}, 123456789ULL);
cp::TreapVector<int> v(cp::PlainSequencePolicy<int>{}, 123456789ULL);
調査した実装

設計時には以下の検証済み・公開実装を比較しました。

KACTL Treap.h: 短いsplit/merge型Implicit Treap。stress-tested
suisen-cp implicit_treap_base.hpp: ノードプール、再利用、反転・遅延・セグ木への拡張
Nyaan splay-lazy-reversible.hpp: 前向き/逆向き集約、遅延伝播、区間反転
Luzhiled's Library: Randomized BST、OrderedMultiSet、Splay/Red-Black Treeの構成
cp-algorithms Treap: 通常TreapとImplicit Treapの基本設計
GNU PBDS公式資料: find_by_order / order_of_key のAPI

本ヘッダはそれらの長所を参考に、API・メモリ管理・方針クラス・テストを新しく組み立てたものです。

テスト

test_balanced_treap.cpp では、次を検証しています。

std::multiset との10万回ランダム比較
std::map との3万回ランダム比較
std::vector との6万回ランダム比較
数値区間更新・集約の5万回ランダム比較
非可換な文字列連結と区間反転
max_right / min_left
AddressSanitizer / UndefinedBehaviorSanitizer
-Wall -Wextra -Wshadow -Wconversion -pedantic

使用例全体は balanced_treap_example.cpp にあります。