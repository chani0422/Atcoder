/*
===============================================================================
TreapVector<T> 操作一覧
===============================================================================

位置をキーとして扱う Implicit Treap。
0-indexedで、任意位置への挿入・削除・アクセスを期待 O(log N) で行う。

-------------------------------------------------------------------------------
1. 宣言・構築
-------------------------------------------------------------------------------

    TreapVector<int> v;
        空のTreapVectorを作る。O(1)

    TreapVector<int> v = {10, 20, 30};
        initializer_listから構築する。O(N)

    vector<int> a = {10, 20, 30};
    TreapVector<int> v(a);
        vectorから構築する。O(N)

    TreapVector<int> v(a.begin(), a.end());
        iterator範囲から構築する。O(N)

    TreapVector<int> v(5, 7);
        7を5個持つ列を作る。O(N)

    TreapVector<pair<ll,ll>> v;
        pairなども格納可能。

    TreapVector<int> u = v;
    u = v;
        コピー構築 / コピー代入。
        削除済みノードを含む内部ノード数に比例する。

    TreapVector<int> u = std::move(v);
    u = std::move(v);
        ムーブ構築 / ムーブ代入。O(1)
        この実装では、ムーブ後のvは空になる。

-------------------------------------------------------------------------------
2. 要素数・容量・初期化
-------------------------------------------------------------------------------

    v.size()
        現在の要素数を返す。O(1)

    v.empty()
        空ならtrueを返す。O(1)

    v.capacity()
        内部のノード配列が確保している容量を返す。O(1)

    v.reserve(n)
        最低n個分の領域を先に確保する。
        最大要素数が分かる場合は、最初に呼ぶと再確保を減らせる。

    v.shrink_to_fit()
        未使用容量の縮小を要求する。

    v.clear()
        全要素を削除する。O(N)

    v.assign(vector<int>{1, 2, 3})
        vectorの内容で作り直す。O(N)

    v.assign(a.begin(), a.end())
        iterator範囲の内容で作り直す。O(N)

    v.swap(u)
    swap(v, u)
        vとuの内容を交換する。O(1)

-------------------------------------------------------------------------------
3. 要素アクセス
-------------------------------------------------------------------------------

    v[i]
        i番目の要素を参照する。期待 O(log N)
        範囲外ならassertに失敗する。

    v.at(i)
        i番目の要素を参照する。期待 O(log N)
        範囲外ならout_of_rangeを投げる。

    v.front()
        先頭要素を参照する。期待 O(log N)

    v.back()
        末尾要素を参照する。期待 O(log N)

    v[i] = x
        i番目の値を変更できる。期待 O(log N)
        ソート済み状態で値を直接変更すると、ソート順が崩れる場合がある。

-------------------------------------------------------------------------------
4. 位置を指定した挿入
-------------------------------------------------------------------------------

    v.insert(pos, x)
        xをpos番目に挿入する。期待 O(log N)
        挿入後、元のpos番目以降は1つ右へ移動する。
        0 <= pos <= v.size() が必要。

    v.emplace(pos, args...)
        T(args...)をpos番目に直接構築して挿入する。期待 O(log N)

    v.push_back(x)
        末尾にxを追加する。期待 O(log N)

    v.emplace_back(args...)
        末尾にT(args...)を直接構築する。期待 O(log N)

    v.push_front(x)
        先頭にxを追加する。期待 O(log N)

    v.emplace_front(args...)
        先頭にT(args...)を直接構築する。期待 O(log N)

    auto it = v.iterator_at(pos);
    auto inserted = v.insert(it, x);
        iterator位置に挿入する。
        戻り値は挿入した要素を指すiterator。

    auto inserted = v.emplace(it, args...);
        iterator位置に直接構築して挿入する。

    注意：#define pb push_back がある環境では v.pb(x) とも書けるが、
          pbはTreapVector自身の機能ではなくマクロによる置換。

-------------------------------------------------------------------------------
5. 削除
-------------------------------------------------------------------------------

    v.erase(pos)
        pos番目を削除する。期待 O(log N)
        0 <= pos < v.size() が必要。

    v.erase(left, right)
        半開区間 [left, right) を削除する。
        期待 O(log N + (right - left))

    auto next = v.erase(it)
        iteratorが指す要素を削除する。
        戻り値は削除後の次の要素を指すiterator。

    v.pop_back()
        末尾を削除する。期待 O(log N)

    v.pop_front()
        先頭を削除する。期待 O(log N)

-------------------------------------------------------------------------------
6. iterator・範囲for
-------------------------------------------------------------------------------

    v.begin() / v.end()
        先頭 / 終端のiteratorを返す。

    v.cbegin() / v.cend()
        const_iteratorを返す。

    v.rbegin() / v.rend()
        逆順iteratorを返す。

    v.crbegin() / v.crend()
        constの逆順iteratorを返す。

    for(const auto& x : v) {
        // 先頭から順番に処理
    }
        全体で O(N)

    for(auto it = v.rbegin(); it != v.rend(); ++it) {
        // 末尾から逆順に処理
    }
        全体で O(N)

    auto it = v.iterator_at(pos)
        pos番目を指すiteratorを作る。期待 O(log N)
        pos == v.size()ならend()を返す。

    v.index_of(it)
        iteratorの現在位置を整数で返す。O(1)

    iteratorで可能な移動
        ++it / it++ / --it / it--

    iteratorで不可能な操作
        it + k / it - k / it[k] / it1 - it2

    理由：このiteratorはbidirectional_iteratorであり、
          random-access iteratorではないため。

    重要：insert / erase / push_* / pop_* 後は、
          それ以前に取得した全iteratorを無効として扱う。

-------------------------------------------------------------------------------
7. 通常のvectorへの変換
-------------------------------------------------------------------------------

    vector<T> a = v.to_vector()
        現在の順番を保った通常のvectorを返す。O(N)

-------------------------------------------------------------------------------
8. ソート
-------------------------------------------------------------------------------

    v.sort()
    v.sort_values()
        昇順にソートする。O(N log N)

    v.sort(greater<T>{})
    v.sort_values(greater<T>{})
        降順にソートする。O(N log N)

    v.revsort()
    v.reverse_sort_values()
        デフォルト比較の逆順、つまり通常は降順にソートする。
        O(N log N)

    v.revsort(comp)
    v.reverse_sort_values(comp)
        compの逆順にソートする。O(N log N)

    禁止：std::sort(v.begin(), v.end())
        TreapVectorには使用しない。v.sort()を使う。

-------------------------------------------------------------------------------
9. ソート済みの場合だけ使用できる検索
-------------------------------------------------------------------------------

以下の操作は、列全体が同じCompareの順にソート済みの場合だけ正しい。
昇順ならCompareを省略できる。

例：
    TreapVector<int> v = {30, 10, 20, 20};
    v.sort();  // {10, 20, 20, 30}

    v.lower_bound_pos(x)
        x以上になる最初の位置を返す。期待 O(log N)
        全要素がx未満ならv.size()を返す。

    v.upper_bound_pos(x)
        xより大きくなる最初の位置を返す。期待 O(log N)
        全要素がx以下ならv.size()を返す。

    auto [left, right] = v.equal_range_pos(x)
        xと等しい要素の範囲 [left, right) を返す。
        期待 O(log N)

    v.count_sorted(x)
        xと等しい要素数を返す。期待 O(log N)

    v.contains_sorted(x)
        xが存在するならtrueを返す。期待 O(log N)

    v.insert_sorted(x)
        ソート順を維持する位置へxを挿入し、その位置を返す。
        期待 O(log N)

    v.erase_one_sorted(x)
        xを1個削除する。
        削除できたらtrue、存在しなければfalseを返す。
        期待 O(log N)

    v.erase_all_sorted(x)
        xをすべて削除し、削除数を返す。
        xの個数をKとして期待 O(log N + K)

降順で使う場合：
    v.sort(greater<int>{});
    int pos = v.lower_bound_pos(x, greater<int>{});
    v.insert_sorted(x, greater<int>{});
    v.erase_one_sorted(x, greater<int>{});

重要：検索時とソート時で、必ず同じCompareを使う。

禁止：std::lower_bound(v.begin(), v.end(), x)
      std::upper_bound(v.begin(), v.end(), x)

これらはbidirectional_iteratorでは位置移動に時間がかかるためO(N)。
必ずlower_bound_pos / upper_bound_posを使う。

-------------------------------------------------------------------------------
10. pairを得点順・番号順で管理する例
-------------------------------------------------------------------------------

    TreapVector<pair<ll,ll>> scores;
    scores.reserve(N);

    for(ll i = 0; i < N; ++i) {
        scores.push_back({S[i], i});
    }

    scores.sort();

    scores.erase_one_sorted({S[t], t});
    S[t] = new_score;
    scores.insert_sorted({S[t], t});

    ll kth_score = scores[k].first;

    // 得点がkth_score未満の人数
    ll less_count = scores.lower_bound_pos({kth_score, -1LL});

-------------------------------------------------------------------------------
11. 計算量一覧
-------------------------------------------------------------------------------

    size / empty / capacity / index_of                 O(1)
    初期vector・iterator範囲からの構築                 O(N)
    operator[] / at / front / back                     期待 O(log N)
    insert / emplace / erase(pos)                      期待 O(log N)
    push_front / push_back / pop_front / pop_back      期待 O(log N)
    erase(left, right)                                 期待 O(log N + K)
    iterator_at                                        期待 O(log N)
    範囲for・to_vector                                 O(N)
    sort / revsort                                     O(N log N)
    lower_bound_pos / upper_bound_pos                  期待 O(log N)
    equal_range_pos / count_sorted / contains_sorted   期待 O(log N)
    insert_sorted / erase_one_sorted                   期待 O(log N)
    erase_all_sorted                                   期待 O(log N + K)

    K = 削除する要素数

-------------------------------------------------------------------------------
12. 最重要注意事項
-------------------------------------------------------------------------------

    ・Treapの高さは乱数で平衡化しているため、計算量には「期待」が付く。
    ・0-indexed。
    ・std::sortではなくv.sort()を使う。
    ・std::lower_bound / std::upper_boundではなく専用関数を使う。
    ・lower_bound_posなどは、ソート済みの場合だけ使用する。
    ・ソート済み状態でv[i]を直接変更すると順序が壊れる可能性がある。
    ・構造変更後は古いiteratorを使わない。
    ・最大要素数が分かるなら最初にreserveする。

===============================================================================
*/

template<class T>
class TreapVector {
    struct Node {
        T val;
        int left = -1;
        int right = -1;
        int size = 1;
        std::uint64_t priority;

        template<class U>
        Node(U&& value, std::uint64_t pri)
            : val(std::forward<U>(value)), priority(pri) {}
    };

    std::vector<Node> nodes_;
    std::vector<int> free_ids_;
    int root_ = -1;
    std::uint64_t rng_state_ = make_seed();

    static std::uint64_t make_seed() {
        return static_cast<std::uint64_t>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        );
    }

    std::uint64_t random_priority() {
        // splitmix64
        std::uint64_t z = (rng_state_ += 0x9e3779b97f4a7c15ULL);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

    int node_size(int node) const {
        return node == -1 ? 0 : nodes_[node].size;
    }

    void pull(int node) {
        assert(node != -1);
        nodes_[node].size =
            1 + node_size(nodes_[node].left) + node_size(nodes_[node].right);
    }

    bool has_higher_priority(int a, int b) const {
        if(nodes_[a].priority != nodes_[b].priority) {
            return nodes_[a].priority > nodes_[b].priority;
        }
        return a > b;
    }

    // node を「先頭 count 個」と「残り」に分割する。
    void split(int node, int count, int& left_tree, int& right_tree) {
        if(node == -1) {
            left_tree = right_tree = -1;
            return;
        }

        int left_size = node_size(nodes_[node].left);

        if(count <= left_size) {
            split(nodes_[node].left, count, left_tree, nodes_[node].left);
            pull(node);
            right_tree = node;
        } else {
            split(
                nodes_[node].right,
                count - left_size - 1,
                nodes_[node].right,
                right_tree
            );
            pull(node);
            left_tree = node;
        }
    }

    int merge(int left_tree, int right_tree) {
        if(left_tree == -1) return right_tree;
        if(right_tree == -1) return left_tree;

        if(has_higher_priority(left_tree, right_tree)) {
            nodes_[left_tree].right =
                merge(nodes_[left_tree].right, right_tree);
            pull(left_tree);
            return left_tree;
        }

        nodes_[right_tree].left =
            merge(left_tree, nodes_[right_tree].left);
        pull(right_tree);
        return right_tree;
    }

    template<class U>
    int create_node(U&& value) {
        std::uint64_t priority = random_priority();

        if(free_ids_.empty()) {
            nodes_.emplace_back(std::forward<U>(value), priority);
            return static_cast<int>(nodes_.size()) - 1;
        }

        int id = free_ids_.back();
        free_ids_.pop_back();

        nodes_[id].val = std::forward<U>(value);
        nodes_[id].left = -1;
        nodes_[id].right = -1;
        nodes_[id].size = 1;
        nodes_[id].priority = priority;
        return id;
    }

    void recycle_node(int node) {
        assert(node != -1);
        nodes_[node].left = -1;
        nodes_[node].right = -1;
        nodes_[node].size = 1;
        free_ids_.push_back(node);
    }

    void recycle_subtree(int subtree_root) {
        if(subtree_root == -1) return;

        std::vector<int> stack = {subtree_root};

        while(!stack.empty()) {
            int node = stack.back();
            stack.pop_back();

            int left = nodes_[node].left;
            int right = nodes_[node].right;

            if(left != -1) stack.push_back(left);
            if(right != -1) stack.push_back(right);

            recycle_node(node);
        }
    }

    int insert_node(int tree, int pos, int new_node) {
        if(tree == -1) return new_node;

        if(has_higher_priority(new_node, tree)) {
            split(
                tree,
                pos,
                nodes_[new_node].left,
                nodes_[new_node].right
            );
            pull(new_node);
            return new_node;
        }

        int left_size = node_size(nodes_[tree].left);

        if(pos <= left_size) {
            nodes_[tree].left =
                insert_node(nodes_[tree].left, pos, new_node);
        } else {
            nodes_[tree].right = insert_node(
                nodes_[tree].right,
                pos - left_size - 1,
                new_node
            );
        }

        pull(tree);
        return tree;
    }

    int erase_node(int tree, int pos, int& erased_node) {
        assert(tree != -1);

        int left_size = node_size(nodes_[tree].left);

        if(pos < left_size) {
            nodes_[tree].left =
                erase_node(nodes_[tree].left, pos, erased_node);
            pull(tree);
            return tree;
        }

        if(pos > left_size) {
            nodes_[tree].right = erase_node(
                nodes_[tree].right,
                pos - left_size - 1,
                erased_node
            );
            pull(tree);
            return tree;
        }

        erased_node = tree;
        return merge(nodes_[tree].left, nodes_[tree].right);
    }

    int kth_node(int k) const {
        assert(0 <= k && k < size());

        int node = root_;

        while(node != -1) {
            int left_size = node_size(nodes_[node].left);

            if(k < left_size) {
                node = nodes_[node].left;
            } else if(k == left_size) {
                return node;
            } else {
                k -= left_size + 1;
                node = nodes_[node].right;
            }
        }

        assert(false);
        return -1;
    }

    void rebuild_sizes() {
        if(root_ == -1) return;

        std::vector<int> order;
        order.reserve(nodes_.size() - free_ids_.size());

        std::vector<int> stack = {root_};
        while(!stack.empty()) {
            int node = stack.back();
            stack.pop_back();
            order.push_back(node);

            if(nodes_[node].left != -1) {
                stack.push_back(nodes_[node].left);
            }
            if(nodes_[node].right != -1) {
                stack.push_back(nodes_[node].right);
            }
        }

        for(auto it = order.rbegin(); it != order.rend(); ++it) {
            pull(*it);
        }
    }

    // values の順番を保ったTreapを O(N) で構築する。
    void build_from_vector(std::vector<T> values) {
        clear();
        reserve(static_cast<int>(values.size()));

        std::vector<int> stack;
        stack.reserve(values.size());

        for(T& value : values) {
            int node = create_node(std::move(value));
            int last_popped = -1;

            while(
                !stack.empty()
                && has_higher_priority(node, stack.back())
            ) {
                last_popped = stack.back();
                stack.pop_back();
            }

            nodes_[node].left = last_popped;

            if(!stack.empty()) {
                nodes_[stack.back()].right = node;
            }

            stack.push_back(node);
        }

        root_ = stack.empty() ? -1 : stack.front();
        rebuild_sizes();
    }

    void move_values_to(std::vector<T>& values) {
        std::vector<int> stack;
        int node = root_;

        while(node != -1 || !stack.empty()) {
            while(node != -1) {
                stack.push_back(node);
                node = nodes_[node].left;
            }

            node = stack.back();
            stack.pop_back();
            values.push_back(std::move(nodes_[node].val));
            node = nodes_[node].right;
        }
    }

    void move_values_from(std::vector<T>& values) {
        std::vector<int> stack;
        int node = root_;
        int pos = 0;

        while(node != -1 || !stack.empty()) {
            while(node != -1) {
                stack.push_back(node);
                node = nodes_[node].left;
            }

            node = stack.back();
            stack.pop_back();
            nodes_[node].val = std::move(values[pos++]);
            node = nodes_[node].right;
        }
    }

public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = int;
    using difference_type = std::ptrdiff_t;

    template<bool IsConst>
    class Iterator {
        using Owner = std::conditional_t<IsConst, const TreapVector, TreapVector>;

        Owner* owner_ = nullptr;
        std::vector<int> path_;
        int pos_ = 0;

        void go_left(int node) {
            while(node != -1) {
                path_.push_back(node);
                node = owner_->nodes_[node].left;
            }
        }

        void go_right(int node) {
            while(node != -1) {
                path_.push_back(node);
                node = owner_->nodes_[node].right;
            }
        }

        void set_position(int pos) {
            assert(owner_ != nullptr);
            assert(0 <= pos && pos <= owner_->size());

            pos_ = pos;
            path_.clear();

            if(pos == owner_->size()) return;

            int node = owner_->root_;
            int k = pos;

            while(node != -1) {
                path_.push_back(node);
                int left_size = owner_->node_size(owner_->nodes_[node].left);

                if(k < left_size) {
                    node = owner_->nodes_[node].left;
                } else if(k == left_size) {
                    return;
                } else {
                    k -= left_size + 1;
                    node = owner_->nodes_[node].right;
                }
            }

            assert(false);
        }

        Iterator(Owner* owner, int pos) : owner_(owner) {
            set_position(pos);
        }

        friend class TreapVector;
        template<bool>
        friend class Iterator;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<IsConst, const T&, T&>;
        using pointer = std::conditional_t<IsConst, const T*, T*>;

        Iterator() = default;

        template<
            bool B = IsConst,
            std::enable_if_t<B, int> = 0
        >
        Iterator(const Iterator<false>& other)
            : owner_(other.owner_), path_(other.path_), pos_(other.pos_) {}

        reference operator*() const {
            assert(owner_ != nullptr);
            assert(pos_ < owner_->size());
            return owner_->nodes_[path_.back()].val;
        }

        pointer operator->() const {
            return std::addressof(operator*());
        }

        Iterator& operator++() {
            assert(owner_ != nullptr);
            assert(pos_ < owner_->size());

            int current = path_.back();

            if(owner_->nodes_[current].right != -1) {
                go_left(owner_->nodes_[current].right);
            } else {
                int child = current;
                path_.pop_back();

                while(
                    !path_.empty()
                    && owner_->nodes_[path_.back()].right == child
                ) {
                    child = path_.back();
                    path_.pop_back();
                }
            }

            ++pos_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator copy = *this;
            ++(*this);
            return copy;
        }

        Iterator& operator--() {
            assert(owner_ != nullptr);
            assert(pos_ > 0);

            if(pos_ == owner_->size()) {
                go_right(owner_->root_);
            } else {
                int current = path_.back();

                if(owner_->nodes_[current].left != -1) {
                    go_right(owner_->nodes_[current].left);
                } else {
                    int child = current;
                    path_.pop_back();

                    while(
                        !path_.empty()
                        && owner_->nodes_[path_.back()].left == child
                    ) {
                        child = path_.back();
                        path_.pop_back();
                    }
                }
            }

            --pos_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator copy = *this;
            --(*this);
            return copy;
        }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.owner_ == b.owner_ && a.pos_ == b.pos_;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return !(a == b);
        }
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    TreapVector() = default;

    TreapVector(std::initializer_list<T> init)
        : TreapVector(std::vector<T>(init)) {}

    explicit TreapVector(std::vector<T> values) {
        build_from_vector(std::move(values));
    }

    template<class InputIterator>
    TreapVector(InputIterator first, InputIterator last) {
        assign(first, last);
    }

    TreapVector(int count, const T& value) {
        assert(count >= 0);
        build_from_vector(std::vector<T>(count, value));
    }

    TreapVector(const TreapVector&) = default;
    TreapVector& operator=(const TreapVector&) = default;

    TreapVector(TreapVector&& other) noexcept
        : nodes_(std::move(other.nodes_)),
          free_ids_(std::move(other.free_ids_)),
          root_(std::exchange(other.root_, -1)),
          rng_state_(other.rng_state_) {}

    TreapVector& operator=(TreapVector&& other) noexcept {
        if(this == &other) return *this;

        nodes_ = std::move(other.nodes_);
        free_ids_ = std::move(other.free_ids_);
        root_ = std::exchange(other.root_, -1);
        rng_state_ = other.rng_state_;
        return *this;
    }

    void swap(TreapVector& other) noexcept {
        nodes_.swap(other.nodes_);
        free_ids_.swap(other.free_ids_);
        std::swap(root_, other.root_);
        std::swap(rng_state_, other.rng_state_);
    }

    int size() const {
        return node_size(root_);
    }

    bool empty() const {
        return root_ == -1;
    }

    int capacity() const {
        return static_cast<int>(nodes_.capacity());
    }

    void reserve(int count) {
        assert(count >= 0);
        nodes_.reserve(count);
        free_ids_.reserve(count);
    }

    void shrink_to_fit() {
        nodes_.shrink_to_fit();
        free_ids_.shrink_to_fit();
    }

    void clear() {
        nodes_.clear();
        free_ids_.clear();
        root_ = -1;
    }

    void assign(std::vector<T> values) {
        build_from_vector(std::move(values));
    }

    template<class InputIterator>
    void assign(InputIterator first, InputIterator last) {
        std::vector<T> values;

        for(; first != last; ++first) {
            values.emplace_back(*first);
        }

        build_from_vector(std::move(values));
    }

    T& operator[](int pos) {
        assert(0 <= pos && pos < size());
        return nodes_[kth_node(pos)].val;
    }

    const T& operator[](int pos) const {
        assert(0 <= pos && pos < size());
        return nodes_[kth_node(pos)].val;
    }

    T& at(int pos) {
        if(pos < 0 || pos >= size()) {
            throw std::out_of_range("TreapVector::at");
        }
        return (*this)[pos];
    }

    const T& at(int pos) const {
        if(pos < 0 || pos >= size()) {
            throw std::out_of_range("TreapVector::at");
        }
        return (*this)[pos];
    }

    T& front() {
        assert(!empty());
        return (*this)[0];
    }

    const T& front() const {
        assert(!empty());
        return (*this)[0];
    }

    T& back() {
        assert(!empty());
        return (*this)[size() - 1];
    }

    const T& back() const {
        assert(!empty());
        return (*this)[size() - 1];
    }

    // 整数位置版は、不要なiteratorを構築しないためvoidを返す。
    template<class U>
    void insert(int pos, U&& value) {
        assert(0 <= pos && pos <= size());
        int node = create_node(std::forward<U>(value));
        root_ = insert_node(root_, pos, node);
    }

    template<class U>
    iterator insert(const_iterator it, U&& value) {
        assert(it.owner_ == this);
        int pos = it.pos_;
        insert(pos, std::forward<U>(value));
        return iterator(this, pos);
    }

    template<class... Args>
    void emplace(int pos, Args&&... args) {
        insert(pos, T(std::forward<Args>(args)...));
    }

    template<class... Args>
    iterator emplace(const_iterator it, Args&&... args) {
        assert(it.owner_ == this);
        int pos = it.pos_;
        insert(pos, T(std::forward<Args>(args)...));
        return iterator(this, pos);
    }

    void push_back(const T& value) {
        int node = create_node(value);
        root_ = merge(root_, node);
    }

    void push_back(T&& value) {
        int node = create_node(std::move(value));
        root_ = merge(root_, node);
    }

    template<class... Args>
    void emplace_back(Args&&... args) {
        push_back(T(std::forward<Args>(args)...));
    }

    void push_front(const T& value) {
        int node = create_node(value);
        root_ = merge(node, root_);
    }

    void push_front(T&& value) {
        int node = create_node(std::move(value));
        root_ = merge(node, root_);
    }

    template<class... Args>
    void emplace_front(Args&&... args) {
        push_front(T(std::forward<Args>(args)...));
    }

    void erase(int pos) {
        assert(0 <= pos && pos < size());

        int erased_node;
        root_ = erase_node(root_, pos, erased_node);
        recycle_node(erased_node);
    }

    iterator erase(const_iterator it) {
        assert(it.owner_ == this);
        int pos = it.pos_;
        erase(pos);
        return iterator(this, pos);
    }

    // 半開区間 [left, right) を削除する。
    void erase(int left, int right) {
        assert(0 <= left && left <= right && right <= size());
        if(left == right) return;

        int prefix;
        int middle_and_suffix;
        int middle;
        int suffix;

        split(root_, left, prefix, middle_and_suffix);
        split(middle_and_suffix, right - left, middle, suffix);
        recycle_subtree(middle);
        root_ = merge(prefix, suffix);
    }

    void pop_back() {
        assert(!empty());
        erase(size() - 1);
    }

    void pop_front() {
        assert(!empty());
        erase(0);
    }

    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, size());
    }

    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    const_iterator end() const {
        return const_iterator(this, size());
    }

    const_iterator cbegin() const {
        return const_iterator(this, 0);
    }

    const_iterator cend() const {
        return const_iterator(this, size());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    iterator iterator_at(int pos) {
        assert(0 <= pos && pos <= size());
        return iterator(this, pos);
    }

    const_iterator iterator_at(int pos) const {
        assert(0 <= pos && pos <= size());
        return const_iterator(this, pos);
    }

    int index_of(const_iterator it) const {
        assert(it.owner_ == this);
        return it.pos_;
    }

    std::vector<T> to_vector() const {
        std::vector<T> values;
        values.reserve(size());

        std::vector<int> stack;
        int node = root_;

        while(node != -1 || !stack.empty()) {
            while(node != -1) {
                stack.push_back(node);
                node = nodes_[node].left;
            }

            node = stack.back();
            stack.pop_back();
            values.push_back(nodes_[node].val);
            node = nodes_[node].right;
        }

        return values;
    }

    template<class Compare = std::less<T>>
    void sort_values(Compare comp = Compare{}) {
        std::vector<T> values;
        values.reserve(size());
        move_values_to(values);

        std::sort(values.begin(), values.end(), comp);
        move_values_from(values);
    }

    template<class Compare = std::less<T>>
    void reverse_sort_values(Compare comp = Compare{}) {
        std::vector<T> values;
        values.reserve(size());
        move_values_to(values);

        std::sort(
            values.begin(),
            values.end(),
            [&](const T& a, const T& b) {
                return comp(b, a);
            }
        );

        move_values_from(values);
    }

    // 以前の呼び方も使用可能。
    template<class Compare = std::less<T>>
    void sort(Compare comp = Compare{}) {
        sort_values(comp);
    }

    template<class Compare = std::less<T>>
    void revsort(Compare comp = Compare{}) {
        reverse_sort_values(comp);
    }

    // 以下は、列がcompの順にソート済みの場合のみ使用可能。
    template<class Compare = std::less<T>>
    int lower_bound_pos(const T& value, Compare comp = Compare{}) const {
        int node = root_;
        int offset = 0;
        int answer = size();

        while(node != -1) {
            int left_size = node_size(nodes_[node].left);

            if(!comp(nodes_[node].val, value)) {
                answer = offset + left_size;
                node = nodes_[node].left;
            } else {
                offset += left_size + 1;
                node = nodes_[node].right;
            }
        }

        return answer;
    }

    template<class Compare = std::less<T>>
    int upper_bound_pos(const T& value, Compare comp = Compare{}) const {
        int node = root_;
        int offset = 0;
        int answer = size();

        while(node != -1) {
            int left_size = node_size(nodes_[node].left);

            if(comp(value, nodes_[node].val)) {
                answer = offset + left_size;
                node = nodes_[node].left;
            } else {
                offset += left_size + 1;
                node = nodes_[node].right;
            }
        }

        return answer;
    }

    template<class Compare = std::less<T>>
    std::pair<int, int> equal_range_pos(
        const T& value,
        Compare comp = Compare{}
    ) const {
        return {
            lower_bound_pos(value, comp),
            upper_bound_pos(value, comp)
        };
    }

    template<class Compare = std::less<T>>
    int count_sorted(const T& value, Compare comp = Compare{}) const {
        auto [left, right] = equal_range_pos(value, comp);
        return right - left;
    }

    template<class Compare = std::less<T>>
    bool contains_sorted(const T& value, Compare comp = Compare{}) const {
        int pos = lower_bound_pos(value, comp);
        if(pos == size()) return false;

        const T& found = (*this)[pos];
        return !comp(found, value) && !comp(value, found);
    }

    template<class Compare = std::less<T>>
    int insert_sorted(const T& value, Compare comp = Compare{}) {
        int pos = lower_bound_pos(value, comp);
        insert(pos, value);
        return pos;
    }

    template<class Compare = std::less<T>>
    int insert_sorted(T&& value, Compare comp = Compare{}) {
        int pos = lower_bound_pos(value, comp);
        insert(pos, std::move(value));
        return pos;
    }

    template<class Compare = std::less<T>>
    bool erase_one_sorted(const T& value, Compare comp = Compare{}) {
        int pos = lower_bound_pos(value, comp);
        if(pos == size()) return false;

        const T& found = (*this)[pos];
        if(comp(found, value) || comp(value, found)) return false;

        erase(pos);
        return true;
    }

    template<class Compare = std::less<T>>
    int erase_all_sorted(const T& value, Compare comp = Compare{}) {
        auto [left, right] = equal_range_pos(value, comp);
        erase(left, right);
        return right - left;
    }
};

template<class T>
void swap(TreapVector<T>& a, TreapVector<T>& b) noexcept {
    a.swap(b);
}