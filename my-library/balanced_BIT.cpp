

#if !__INCLUDE_LEVEL__
#include __FILE__

#pragma once

// balanced_treap.hpp
// A self-contained C++17 competitive-programming library.
//
// Containers:
//   cp::TreapSet<T>                 : ordered set + order statistics
//   cp::TreapMultiSet<T>            : ordered multiset + order statistics
//   cp::TreapMap<Key, Value>         : ordered map + order statistics
//   cp::TreapVector<T>              : dynamic sequence (insert/erase/reverse/...)
//   cp::FoldableTreapVector<T, Op>  : dynamic sequence + range fold
//   cp::AffineSumTreapVector<T>     : range affine update + range sum
//   cp::NumericTreapVector<T>       : affine update + sum/min/max
//
// All logarithmic bounds are expected O(log N), because a randomized Treap is
// used. Bulk construction is O(N) after the input order is fixed.

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace cp {

namespace treap_detail {

inline constexpr std::uint64_t default_seed = 0x243f6a8885a308d3ULL;

class SplitMix64 {
 public:
  explicit SplitMix64(std::uint64_t seed = default_seed) : state_(seed) {}

  std::uint64_t operator()() {
    std::uint64_t z = (state_ += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  }

 private:
  std::uint64_t state_;
};

inline void require_index(std::size_t index, std::size_t size,
                          const char* where) {
  if (index >= size) throw std::out_of_range(where);
}

inline void require_insert_position(std::size_t index, std::size_t size,
                                    const char* where) {
  if (index > size) throw std::out_of_range(where);
}

inline void require_range(std::size_t l, std::size_t r, std::size_t size,
                          const char* where) {
  if (l > r || r > size) throw std::out_of_range(where);
}

}  // namespace treap_detail

// ============================================================================
// 1. Key-ordered Treap: set / multiset + order statistics
// ============================================================================

template <class T, class Compare = std::less<T>, bool AllowDuplicates = true>
class OrderStatisticTreap {
 public:
  using value_type = T;
  using size_type = std::size_t;

  explicit OrderStatisticTreap(
      Compare comp = Compare{},
      std::uint64_t seed = treap_detail::default_seed)
      : comp_(std::move(comp)), rng_(seed) {}

  template <class InputIt>
  OrderStatisticTreap(InputIt first, InputIt last, Compare comp = Compare{},
                      std::uint64_t seed = treap_detail::default_seed)
      : comp_(std::move(comp)), rng_(seed) {
    assign(first, last);
  }

  OrderStatisticTreap(std::initializer_list<T> init,
                      Compare comp = Compare{},
                      std::uint64_t seed = treap_detail::default_seed)
      : OrderStatisticTreap(init.begin(), init.end(), std::move(comp), seed) {}

  [[nodiscard]] bool empty() const noexcept { return root_ == null; }
  [[nodiscard]] size_type size() const noexcept { return subtree_size(root_); }
  [[nodiscard]] size_type distinct_size() const noexcept {
    return distinct_size_;
  }

  void reserve(size_type node_count) {
    nodes_.reserve(node_count);
    free_.reserve(node_count);
  }

  void clear() noexcept {
    root_ = null;
    distinct_size_ = 0;
    nodes_.clear();
    free_.clear();
  }

  // For a set: false when key already existed.
  // For a multiset: always true.
  bool insert(T key) {
    bool inserted = false;
    root_ = insert_rec(root_, key, inserted);
    return inserted;
  }

  // STL-like erase(key): removes every occurrence and returns the count.
  size_type erase(const T& key) { return erase_all<T>(key); }

  template <class Key>
  size_type erase(const Key& key) {
    return erase_all(key);
  }

  bool erase_one(const T& key) { return erase_one<T>(key); }

  template <class Key>
  bool erase_one(const Key& key) {
    bool erased = false;
    root_ = erase_one_rec(root_, key, erased);
    return erased;
  }

  size_type erase_all(const T& key) { return erase_all<T>(key); }

  template <class Key>
  size_type erase_all(const Key& key) {
    size_type erased = 0;
    root_ = erase_all_rec(root_, key, erased);
    return erased;
  }

  [[nodiscard]] bool contains(const T& key) const {
    return contains<T>(key);
  }

  template <class Key>
  [[nodiscard]] bool contains(const Key& key) const {
    return find_node(key) != null;
  }

  [[nodiscard]] size_type count(const T& key) const { return count<T>(key); }

  template <class Key>
  [[nodiscard]] size_type count(const Key& key) const {
    const int t = find_node(key);
    return t == null ? 0 : node(t).frequency;
  }

  // Number of elements strictly smaller than key.
  [[nodiscard]] size_type order_of_key(const T& key) const {
    return order_of_key<T>(key);
  }

  template <class Key>
  [[nodiscard]] size_type order_of_key(const Key& key) const {
    size_type result = 0;
    int t = root_;
    while (t != null) {
      if (comp_(node(t).key, key)) {
        result += subtree_size(node(t).left) + node(t).frequency;
        t = node(t).right;
      } else {
        t = node(t).left;
      }
    }
    return result;
  }

  // Number of elements <= key.
  [[nodiscard]] size_type order_of_key_upper(const T& key) const {
    return order_of_key_upper<T>(key);
  }

  template <class Key>
  [[nodiscard]] size_type order_of_key_upper(const Key& key) const {
    size_type result = 0;
    int t = root_;
    while (t != null) {
      if (comp_(key, node(t).key)) {
        t = node(t).left;
      } else {
        result += subtree_size(node(t).left) + node(t).frequency;
        t = node(t).right;
      }
    }
    return result;
  }

  // Number of elements in [lower, upper) according to Compare.
  template <class Lower, class Upper>
  [[nodiscard]] size_type count_range(const Lower& lower,
                                      const Upper& upper) const {
    const size_type left = order_of_key(lower);
    const size_type right = order_of_key(upper);
    return right >= left ? right - left : 0;
  }

  template <class Key>
  [[nodiscard]] std::pair<size_type, size_type> equal_range_order(
      const Key& key) const {
    return {order_of_key(key), order_of_key_upper(key)};
  }

  // 0-indexed k-th smallest value.
  [[nodiscard]] std::optional<T> find_by_order(size_type k) const {
    if (k >= size()) return std::nullopt;
    int t = root_;
    while (t != null) {
      const size_type left_size = subtree_size(node(t).left);
      if (k < left_size) {
        t = node(t).left;
      } else if (k < left_size + node(t).frequency) {
        return node(t).key;
      } else {
        k -= left_size + node(t).frequency;
        t = node(t).right;
      }
    }
    return std::nullopt;
  }

  [[nodiscard]] T kth(size_type k) const {
    auto result = find_by_order(k);
    if (!result) throw std::out_of_range("OrderStatisticTreap::kth");
    return std::move(*result);
  }

  [[nodiscard]] std::optional<T> median_lower() const {
    return empty() ? std::nullopt : find_by_order((size() - 1) / 2);
  }

  [[nodiscard]] std::optional<T> median_upper() const {
    return empty() ? std::nullopt : find_by_order(size() / 2);
  }

  std::optional<T> extract_by_order(size_type k) {
    auto value = find_by_order(k);
    if (value) erase_one(*value);
    return value;
  }

  [[nodiscard]] std::optional<T> min() const {
    if (root_ == null) return std::nullopt;
    int t = root_;
    while (node(t).left != null) t = node(t).left;
    return node(t).key;
  }

  [[nodiscard]] std::optional<T> max() const {
    if (root_ == null) return std::nullopt;
    int t = root_;
    while (node(t).right != null) t = node(t).right;
    return node(t).key;
  }

  std::optional<T> pop_min() {
    auto value = min();
    if (value) erase_one(*value);
    return value;
  }

  std::optional<T> pop_max() {
    auto value = max();
    if (value) erase_one(*value);
    return value;
  }

  // First value >= key.
  [[nodiscard]] std::optional<T> lower_bound(const T& key) const {
    return lower_bound<T>(key);
  }

  template <class Key>
  [[nodiscard]] std::optional<T> lower_bound(const Key& key) const {
    int t = root_, answer = null;
    while (t != null) {
      if (!comp_(node(t).key, key)) {
        answer = t;
        t = node(t).left;
      } else {
        t = node(t).right;
      }
    }
    return answer == null ? std::nullopt
                          : std::optional<T>{node(answer).key};
  }

  // First value > key.
  [[nodiscard]] std::optional<T> upper_bound(const T& key) const {
    return upper_bound<T>(key);
  }

  template <class Key>
  [[nodiscard]] std::optional<T> upper_bound(const Key& key) const {
    int t = root_, answer = null;
    while (t != null) {
      if (comp_(key, node(t).key)) {
        answer = t;
        t = node(t).left;
      } else {
        t = node(t).right;
      }
    }
    return answer == null ? std::nullopt
                          : std::optional<T>{node(answer).key};
  }

  // Greatest value < key.
  [[nodiscard]] std::optional<T> predecessor(const T& key) const {
    return predecessor<T>(key);
  }

  template <class Key>
  [[nodiscard]] std::optional<T> predecessor(const Key& key) const {
    int t = root_, answer = null;
    while (t != null) {
      if (comp_(node(t).key, key)) {
        answer = t;
        t = node(t).right;
      } else {
        t = node(t).left;
      }
    }
    return answer == null ? std::nullopt
                          : std::optional<T>{node(answer).key};
  }

  // Greatest value <= key.
  [[nodiscard]] std::optional<T> floor(const T& key) const {
    return floor<T>(key);
  }

  template <class Key>
  [[nodiscard]] std::optional<T> floor(const Key& key) const {
    const size_type count_not_greater = order_of_key_upper(key);
    return count_not_greater == 0 ? std::nullopt
                                  : find_by_order(count_not_greater - 1);
  }

  // Smallest value >= key.
  [[nodiscard]] std::optional<T> ceiling(const T& key) const {
    return ceiling<T>(key);
  }

  template <class Key>
  [[nodiscard]] std::optional<T> ceiling(const Key& key) const {
    return lower_bound(key);
  }

  // Smallest value > key.
  [[nodiscard]] std::optional<T> successor(const T& key) const {
    return successor<T>(key);
  }

  template <class Key>
  [[nodiscard]] std::optional<T> successor(const Key& key) const {
    return upper_bound(key);
  }

  [[nodiscard]] std::vector<T> to_vector() const {
    std::vector<T> result;
    result.reserve(size());
    std::vector<int> stack;
    int current = root_;
    while (current != null || !stack.empty()) {
      while (current != null) {
        stack.push_back(current);
        current = node(current).left;
      }
      current = stack.back();
      stack.pop_back();
      for (size_type i = 0; i < node(current).frequency; ++i) {
        result.push_back(node(current).key);
      }
      current = node(current).right;
    }
    return result;
  }

  [[nodiscard]] std::vector<T> distinct_to_vector() const {
    std::vector<T> result;
    result.reserve(distinct_size_);
    std::vector<int> stack;
    int current = root_;
    while (current != null || !stack.empty()) {
      while (current != null) {
        stack.push_back(current);
        current = node(current).left;
      }
      current = stack.back();
      stack.pop_back();
      result.push_back(node(current).key);
      current = node(current).right;
    }
    return result;
  }

  template <class Function>
  void for_each(Function&& function) const {
    std::vector<int> stack;
    int current = root_;
    while (current != null || !stack.empty()) {
      while (current != null) {
        stack.push_back(current);
        current = node(current).left;
      }
      current = stack.back();
      stack.pop_back();
      for (size_type i = 0; i < node(current).frequency; ++i) {
        std::invoke(function, node(current).key);
      }
      current = node(current).right;
    }
  }

  // Sorts the input, compresses duplicates, then builds in linear time from
  // the sorted distinct keys. Total: O(N log N), dominated by sorting.
  template <class InputIt>
  void assign(InputIt first, InputIt last) {
    std::vector<T> values(first, last);
    std::sort(values.begin(), values.end(), comp_);
    assign_sorted(values.begin(), values.end());
  }

  // Input must already be sorted according to Compare.
  template <class InputIt>
  void assign_sorted(InputIt first, InputIt last_it) {
    clear();
    std::vector<std::pair<T, size_type>> compressed;
    for (; first != last_it; ++first) {
      if (compressed.empty() || !equivalent(compressed.back().first, *first)) {
        compressed.emplace_back(*first, 1);
      } else if constexpr (AllowDuplicates) {
        ++compressed.back().second;
      }
    }
    reserve(compressed.size());
    if (compressed.empty()) return;

    std::vector<int> stack;
    stack.reserve(compressed.size());
    for (auto& entry : compressed) {
      const int current = make_node(std::move(entry.first), entry.second);
      int previous_root = null;
      while (!stack.empty() &&
             node(stack.back()).priority < node(current).priority) {
        previous_root = stack.back();
        stack.pop_back();
      }
      node(current).left = previous_root;
      if (!stack.empty()) node(stack.back()).right = current;
      stack.push_back(current);
    }
    root_ = stack.front();
    distinct_size_ = compressed.size();
    recalculate_subtree(root_);
  }

  [[nodiscard]] size_type height() const {
    if (root_ == null) return 0;
    size_type answer = 0;
    std::vector<std::pair<int, size_type>> stack{{root_, 1}};
    while (!stack.empty()) {
      const auto [t, depth] = stack.back();
      stack.pop_back();
      answer = std::max(answer, depth);
      if (node(t).left != null) stack.emplace_back(node(t).left, depth + 1);
      if (node(t).right != null)
        stack.emplace_back(node(t).right, depth + 1);
    }
    return answer;
  }

  // O(N) debug check: BST order, heap order, multiplicities and subtree sizes.
  [[nodiscard]] bool validate() const {
    size_type seen_distinct = 0;
    const auto result = validate_rec(root_, nullptr, nullptr, seen_distinct);
    return result.ok && result.size == size() &&
           seen_distinct == distinct_size_;
  }

 private:
  static constexpr int null = -1;

  struct Node {
    T key;
    std::uint64_t priority;
    int left = null;
    int right = null;
    size_type subtree_size = 1;
    size_type frequency = 1;

    Node(T value, std::uint64_t p, size_type freq)
        : key(std::move(value)),
          priority(p),
          subtree_size(freq),
          frequency(freq) {}
  };

  struct ValidationResult {
    bool ok;
    size_type size;
  };

  Compare comp_;
  treap_detail::SplitMix64 rng_;
  std::vector<Node> nodes_;
  std::vector<int> free_;
  int root_ = null;
  size_type distinct_size_ = 0;

  Node& node(int index) { return nodes_[static_cast<size_type>(index)]; }
  const Node& node(int index) const {
    return nodes_[static_cast<size_type>(index)];
  }

  [[nodiscard]] size_type subtree_size(int t) const noexcept {
    return t == null ? 0 : node(t).subtree_size;
  }

  template <class A, class B>
  [[nodiscard]] bool equivalent(const A& a, const B& b) const {
    return !comp_(a, b) && !comp_(b, a);
  }

  int make_node(T value, size_type frequency = 1) {
    const std::uint64_t priority = rng_();
    if (free_.empty()) {
      const int index = static_cast<int>(nodes_.size());
      nodes_.emplace_back(std::move(value), priority, frequency);
      return index;
    }
    const int index = free_.back();
    free_.pop_back();
    node(index) = Node(std::move(value), priority, frequency);
    return index;
  }

  void release_node(int t) { free_.push_back(t); }

  void pull(int t) {
    node(t).subtree_size = subtree_size(node(t).left) + node(t).frequency +
                           subtree_size(node(t).right);
  }

  int rotate_right(int t) {
    const int child = node(t).left;
    node(t).left = node(child).right;
    node(child).right = t;
    pull(t);
    pull(child);
    return child;
  }

  int rotate_left(int t) {
    const int child = node(t).right;
    node(t).right = node(child).left;
    node(child).left = t;
    pull(t);
    pull(child);
    return child;
  }

  int insert_rec(int t, T& key, bool& inserted) {
    if (t == null) {
      inserted = true;
      ++distinct_size_;
      return make_node(std::move(key));
    }
    if (equivalent(key, node(t).key)) {
      if constexpr (AllowDuplicates) {
        ++node(t).frequency;
        inserted = true;
        pull(t);
      }
      return t;
    }

    const bool go_left = comp_(key, node(t).key);
    if (go_left) {
      const int child = insert_rec(node(t).left, key, inserted);
      node(t).left = child;
      if (node(child).priority > node(t).priority) t = rotate_right(t);
    } else {
      const int child = insert_rec(node(t).right, key, inserted);
      node(t).right = child;
      if (node(child).priority > node(t).priority) t = rotate_left(t);
    }
    pull(t);
    return t;
  }

  int merge_nodes(int left, int right) {
    if (left == null) return right;
    if (right == null) return left;
    if (node(left).priority >= node(right).priority) {
      node(left).right = merge_nodes(node(left).right, right);
      pull(left);
      return left;
    }
    node(right).left = merge_nodes(left, node(right).left);
    pull(right);
    return right;
  }

  template <class Key>
  int erase_one_rec(int t, const Key& key, bool& erased) {
    if (t == null) return null;
    if (equivalent(key, node(t).key)) {
      erased = true;
      if (node(t).frequency > 1) {
        --node(t).frequency;
        pull(t);
        return t;
      }
      const int result = merge_nodes(node(t).left, node(t).right);
      release_node(t);
      --distinct_size_;
      return result;
    }
    if (comp_(key, node(t).key)) {
      node(t).left = erase_one_rec(node(t).left, key, erased);
    } else {
      node(t).right = erase_one_rec(node(t).right, key, erased);
    }
    pull(t);
    return t;
  }

  template <class Key>
  int erase_all_rec(int t, const Key& key, size_type& erased) {
    if (t == null) return null;
    if (equivalent(key, node(t).key)) {
      erased = node(t).frequency;
      const int result = merge_nodes(node(t).left, node(t).right);
      release_node(t);
      --distinct_size_;
      return result;
    }
    if (comp_(key, node(t).key)) {
      node(t).left = erase_all_rec(node(t).left, key, erased);
    } else {
      node(t).right = erase_all_rec(node(t).right, key, erased);
    }
    pull(t);
    return t;
  }

  template <class Key>
  [[nodiscard]] int find_node(const Key& key) const {
    int t = root_;
    while (t != null) {
      if (equivalent(key, node(t).key)) return t;
      t = comp_(key, node(t).key) ? node(t).left : node(t).right;
    }
    return null;
  }

  void recalculate_subtree(int root) {
    if (root == null) return;
    std::vector<std::pair<int, bool>> stack{{root, false}};
    while (!stack.empty()) {
      const auto [t, visited] = stack.back();
      stack.pop_back();
      if (visited) {
        pull(t);
      } else {
        stack.emplace_back(t, true);
        if (node(t).right != null) stack.emplace_back(node(t).right, false);
        if (node(t).left != null) stack.emplace_back(node(t).left, false);
      }
    }
  }

  ValidationResult validate_rec(int t, const T* lower, const T* upper,
                                size_type& seen_distinct) const {
    if (t == null) return {true, 0};
    const Node& current = node(t);
    if (current.frequency == 0) return {false, 0};
    if (lower && !comp_(*lower, current.key)) return {false, 0};
    if (upper && !comp_(current.key, *upper)) return {false, 0};
    if (current.left != null &&
        node(current.left).priority > current.priority) {
      return {false, 0};
    }
    if (current.right != null &&
        node(current.right).priority > current.priority) {
      return {false, 0};
    }
    ++seen_distinct;
    const auto left = validate_rec(current.left, lower, &current.key,
                                   seen_distinct);
    const auto right = validate_rec(current.right, &current.key, upper,
                                    seen_distinct);
    const size_type expected = left.size + current.frequency + right.size;
    return {left.ok && right.ok && expected == current.subtree_size, expected};
  }
};

template <class T, class Compare = std::less<T>>
using TreapSet = OrderStatisticTreap<T, Compare, false>;

template <class T, class Compare = std::less<T>>
using TreapMultiSet = OrderStatisticTreap<T, Compare, true>;

// Ordered map with order statistics. Mapped values are returned by value;
// update them with insert_or_assign() or modify().
template <class Key, class Mapped, class Compare = std::less<Key>>
class TreapMap {
 private:
  struct Entry {
    Key key;
    Mapped mapped;
  };

  struct EntryCompare {
    Compare comp;

    bool operator()(const Entry& a, const Entry& b) const {
      return comp(a.key, b.key);
    }
    template <class K>
    bool operator()(const Entry& a, const K& b) const {
      return comp(a.key, b);
    }
    template <class K>
    bool operator()(const K& a, const Entry& b) const {
      return comp(a, b.key);
    }
  };

  using Tree = OrderStatisticTreap<Entry, EntryCompare, false>;

 public:
  using key_type = Key;
  using mapped_type = Mapped;
  using value_type = std::pair<Key, Mapped>;
  using size_type = std::size_t;

  explicit TreapMap(Compare comp = Compare{},
                    std::uint64_t seed = treap_detail::default_seed)
      : comp_(comp), tree_(EntryCompare{std::move(comp)}, seed) {}

  TreapMap(std::initializer_list<value_type> init,
           Compare comp = Compare{},
           std::uint64_t seed = treap_detail::default_seed)
      : TreapMap(std::move(comp), seed) {
    reserve(init.size());
    for (const auto& [key, mapped] : init) insert(key, mapped);
  }

  [[nodiscard]] bool empty() const noexcept { return tree_.empty(); }
  [[nodiscard]] size_type size() const noexcept { return tree_.size(); }
  void reserve(size_type count) { tree_.reserve(count); }
  void clear() noexcept { tree_.clear(); }

  bool insert(Key key, Mapped mapped) {
    return tree_.insert(Entry{std::move(key), std::move(mapped)});
  }

  bool insert(value_type value) {
    return insert(std::move(value.first), std::move(value.second));
  }

  // Returns true when a new key was inserted, false when an old value changed.
  bool insert_or_assign(Key key, Mapped mapped) {
    const bool inserted = insert(key, mapped);
    if (inserted) return true;
    tree_.erase_one(key);
    tree_.insert(Entry{std::move(key), std::move(mapped)});
    return false;
  }

  template <class K>
  bool erase(const K& key) {
    return tree_.erase_one(key);
  }

  template <class K>
  [[nodiscard]] bool contains(const K& key) const {
    return tree_.contains(key);
  }

  template <class K>
  [[nodiscard]] std::optional<Mapped> get(const K& key) const {
    const auto entry = tree_.lower_bound(key);
    if (!entry || !equivalent_key(entry->key, key)) return std::nullopt;
    return entry->mapped;
  }

  template <class K>
  [[nodiscard]] Mapped at(const K& key) const {
    auto result = get(key);
    if (!result) throw std::out_of_range("TreapMap::at");
    return std::move(*result);
  }

  template <class K>
  [[nodiscard]] Mapped get_or(const K& key, Mapped default_value) const {
    auto result = get(key);
    return result ? std::move(*result) : std::move(default_value);
  }

  template <class K, class Function>
  bool modify(const K& key, Function&& function) {
    auto old = get(key);
    if (!old) return false;
    Mapped updated = std::move(*old);
    using result_type = std::invoke_result_t<Function, Mapped&>;
    if constexpr (std::is_void_v<result_type>) {
      std::invoke(function, updated);
    } else {
      updated = std::invoke(function, updated);
    }
    Key stored_key = tree_.lower_bound(key)->key;
    tree_.erase_one(key);
    tree_.insert(Entry{std::move(stored_key), std::move(updated)});
    return true;
  }

  template <class K>
  [[nodiscard]] size_type order_of_key(const K& key) const {
    return tree_.order_of_key(key);
  }

  template <class Lower, class Upper>
  [[nodiscard]] size_type count_range(const Lower& lower,
                                      const Upper& upper) const {
    return tree_.count_range(lower, upper);
  }

  [[nodiscard]] std::optional<value_type> find_by_order(size_type k) const {
    const auto entry = tree_.find_by_order(k);
    if (!entry) return std::nullopt;
    return value_type{entry->key, entry->mapped};
  }

  [[nodiscard]] value_type kth(size_type k) const {
    const Entry entry = tree_.kth(k);
    return {entry.key, entry.mapped};
  }

  template <class K>
  [[nodiscard]] std::optional<value_type> lower_bound(const K& key) const {
    return convert(tree_.lower_bound(key));
  }

  template <class K>
  [[nodiscard]] std::optional<value_type> upper_bound(const K& key) const {
    return convert(tree_.upper_bound(key));
  }

  template <class K>
  [[nodiscard]] std::optional<value_type> predecessor(const K& key) const {
    return convert(tree_.predecessor(key));
  }

  template <class K>
  [[nodiscard]] std::optional<value_type> successor(const K& key) const {
    return convert(tree_.successor(key));
  }

  [[nodiscard]] std::optional<value_type> min() const {
    return convert(tree_.min());
  }

  [[nodiscard]] std::optional<value_type> max() const {
    return convert(tree_.max());
  }

  [[nodiscard]] std::vector<value_type> to_vector() const {
    const auto entries = tree_.to_vector();
    std::vector<value_type> result;
    result.reserve(entries.size());
    for (const Entry& entry : entries)
      result.emplace_back(entry.key, entry.mapped);
    return result;
  }

  [[nodiscard]] std::vector<Key> keys() const {
    const auto entries = tree_.to_vector();
    std::vector<Key> result;
    result.reserve(entries.size());
    for (const Entry& entry : entries) result.push_back(entry.key);
    return result;
  }

  [[nodiscard]] std::vector<Mapped> values() const {
    const auto entries = tree_.to_vector();
    std::vector<Mapped> result;
    result.reserve(entries.size());
    for (const Entry& entry : entries) result.push_back(entry.mapped);
    return result;
  }

  [[nodiscard]] bool validate() const { return tree_.validate(); }
  [[nodiscard]] size_type height() const { return tree_.height(); }

 private:
  Compare comp_;
  Tree tree_;

  template <class A, class B>
  [[nodiscard]] bool equivalent_key(const A& a, const B& b) const {
    return !comp_(a, b) && !comp_(b, a);
  }

  static std::optional<value_type> convert(const std::optional<Entry>& entry) {
    if (!entry) return std::nullopt;
    return value_type{entry->key, entry->mapped};
  }
};

// ============================================================================
// 2. Policies for the implicit (position-ordered) Treap
// ============================================================================

struct EmptyAggregate {};
struct NoTag {};

template <class T>
struct PlainSequencePolicy {
  using aggregate_type = EmptyAggregate;
  using tag_type = NoTag;

  aggregate_type identity() const { return {}; }
  aggregate_type make_aggregate(const T&) const { return {}; }
  aggregate_type combine(const aggregate_type&, const aggregate_type&) const {
    return {};
  }
  void apply_value(T&, const tag_type&) const {}
  void apply_aggregate(aggregate_type&, const tag_type&, std::size_t) const {}
  tag_type compose(const tag_type&, const tag_type&) const { return {}; }
};

template <class T, class Op>
struct FoldSequencePolicy {
  using aggregate_type = T;
  using tag_type = NoTag;

  T identity_value{};
  Op op{};

  aggregate_type identity() const { return identity_value; }
  aggregate_type make_aggregate(const T& value) const { return value; }
  aggregate_type combine(const aggregate_type& left,
                         const aggregate_type& right) const {
    return std::invoke(op, left, right);
  }
  void apply_value(T&, const tag_type&) const {}
  void apply_aggregate(aggregate_type&, const tag_type&, std::size_t) const {}
  tag_type compose(const tag_type&, const tag_type&) const { return {}; }
};

template <class T>
struct AffineTag {
  T multiply = T{1};
  T add = T{};
};

template <class T>
struct SumAggregate {
  T sum{};
  std::size_t count = 0;
};

template <class T>
struct AffineSumPolicy {
  using aggregate_type = SumAggregate<T>;
  using tag_type = AffineTag<T>;

  aggregate_type identity() const { return {}; }
  aggregate_type make_aggregate(const T& value) const { return {value, 1}; }

  aggregate_type combine(const aggregate_type& left,
                         const aggregate_type& right) const {
    return {left.sum + right.sum, left.count + right.count};
  }

  void apply_value(T& value, const tag_type& tag) const {
    value = tag.multiply * value + tag.add;
  }

  void apply_aggregate(aggregate_type& aggregate, const tag_type& tag,
                       std::size_t length) const {
    aggregate.sum = tag.multiply * aggregate.sum + tag.add * T(length);
  }

  // Result means: apply older first, then newer.
  tag_type compose(const tag_type& newer, const tag_type& older) const {
    return {newer.multiply * older.multiply,
            newer.multiply * older.add + newer.add};
  }
};

template <class T>
struct NumericAggregate {
  T sum{};
  T minimum{};
  T maximum{};
  std::size_t count = 0;
};

template <class T>
struct NumericAffinePolicy {
  using aggregate_type = NumericAggregate<T>;
  using tag_type = AffineTag<T>;

  aggregate_type identity() const { return {}; }
  aggregate_type make_aggregate(const T& value) const {
    return {value, value, value, 1};
  }

  aggregate_type combine(const aggregate_type& left,
                         const aggregate_type& right) const {
    if (left.count == 0) return right;
    if (right.count == 0) return left;
    return {left.sum + right.sum, std::min(left.minimum, right.minimum),
            std::max(left.maximum, right.maximum),
            left.count + right.count};
  }

  void apply_value(T& value, const tag_type& tag) const {
    value = tag.multiply * value + tag.add;
  }

  void apply_aggregate(aggregate_type& aggregate, const tag_type& tag,
                       std::size_t length) const {
    if (aggregate.count == 0) return;
    aggregate.sum = tag.multiply * aggregate.sum + tag.add * T(length);
    const T old_min = aggregate.minimum;
    const T old_max = aggregate.maximum;
    if (tag.multiply < T{}) {
      aggregate.minimum = tag.multiply * old_max + tag.add;
      aggregate.maximum = tag.multiply * old_min + tag.add;
    } else {
      aggregate.minimum = tag.multiply * old_min + tag.add;
      aggregate.maximum = tag.multiply * old_max + tag.add;
    }
  }

  tag_type compose(const tag_type& newer, const tag_type& older) const {
    return {newer.multiply * older.multiply,
            newer.multiply * older.add + newer.add};
  }
};

// ============================================================================
// 3. Position-ordered implicit Treap
// ============================================================================

// Custom Policy contract:
//   using aggregate_type;
//   using tag_type;
//   aggregate_type identity() const;
//   aggregate_type make_aggregate(const T&) const;
//   aggregate_type combine(const aggregate_type&, const aggregate_type&) const;
//   void apply_value(T&, const tag_type&) const;
//   void apply_aggregate(aggregate_type&, const tag_type&, size_t len) const;
//   tag_type compose(const tag_type& newer, const tag_type& older) const;
//
// The action must be pointwise, so applying a tag and reversing a range commute.
template <class T, class Policy = PlainSequencePolicy<T>>
class ImplicitTreap {
 public:
  using value_type = T;
  using policy_type = Policy;
  using aggregate_type = typename Policy::aggregate_type;
  using tag_type = typename Policy::tag_type;
  using size_type = std::size_t;

  explicit ImplicitTreap(
      Policy policy = Policy{},
      std::uint64_t seed = treap_detail::default_seed)
      : policy_(std::move(policy)), rng_(seed) {}

  explicit ImplicitTreap(
      std::vector<T> values, Policy policy = Policy{},
      std::uint64_t seed = treap_detail::default_seed)
      : policy_(std::move(policy)), rng_(seed) {
    assign(std::move(values));
  }

  template <class InputIt>
  ImplicitTreap(InputIt first, InputIt last, Policy policy = Policy{},
                std::uint64_t seed = treap_detail::default_seed)
      : policy_(std::move(policy)), rng_(seed) {
    assign(first, last);
  }

  ImplicitTreap(std::initializer_list<T> init, Policy policy = Policy{},
                std::uint64_t seed = treap_detail::default_seed)
      : ImplicitTreap(init.begin(), init.end(), std::move(policy), seed) {}

  [[nodiscard]] bool empty() const noexcept { return root_ == null; }
  [[nodiscard]] size_type size() const noexcept { return subtree_size(root_); }

  void reserve(size_type node_count) {
    nodes_.reserve(node_count);
    free_.reserve(node_count);
  }

  void clear() noexcept {
    root_ = null;
    nodes_.clear();
    free_.clear();
  }

  void assign(std::vector<T> values) {
    clear();
    reserve(values.size());
    root_ = build_nodes(std::move(values));
  }

  template <class InputIt>
  void assign(InputIt first, InputIt last) {
    assign(std::vector<T>(first, last));
  }

  [[nodiscard]] T get(size_type position) {
    treap_detail::require_index(position, size(), "ImplicitTreap::get");
    return get_rec(root_, position);
  }

  [[nodiscard]] T at(size_type position) { return get(position); }
  [[nodiscard]] T operator[](size_type position) { return get(position); }

  [[nodiscard]] std::optional<T> try_get(size_type position) {
    if (position >= size()) return std::nullopt;
    return get_rec(root_, position);
  }

  [[nodiscard]] T front() {
    if (empty()) throw std::out_of_range("ImplicitTreap::front");
    return get_rec(root_, 0);
  }

  [[nodiscard]] T back() {
    if (empty()) throw std::out_of_range("ImplicitTreap::back");
    return get_rec(root_, size() - 1);
  }

  void set(size_type position, T value) {
    treap_detail::require_index(position, size(), "ImplicitTreap::set");
    root_ = set_rec(root_, position, value);
  }

  template <class Function>
  void modify(size_type position, Function&& function) {
    treap_detail::require_index(position, size(), "ImplicitTreap::modify");
    root_ = modify_rec(root_, position, function);
  }

  void insert(size_type position, T value) {
    treap_detail::require_insert_position(position, size(),
                                          "ImplicitTreap::insert");
    const int fresh = make_node(std::move(value));
    auto [left, right] = split_nodes(root_, position);
    root_ = merge_nodes(merge_nodes(left, fresh), right);
  }

  void push_front(T value) { insert(0, std::move(value)); }
  void push_back(T value) { insert(size(), std::move(value)); }

  template <class InputIt>
  void insert_range(size_type position, InputIt first, InputIt last) {
    insert_range(position, std::vector<T>(first, last));
  }

  void insert_range(size_type position, std::vector<T> values) {
    treap_detail::require_insert_position(position, size(),
                                          "ImplicitTreap::insert_range");
    if (values.empty()) return;
    const int middle = build_nodes(std::move(values));
    auto [left, right] = split_nodes(root_, position);
    root_ = merge_nodes(merge_nodes(left, middle), right);
  }

  T erase(size_type position) {
    treap_detail::require_index(position, size(), "ImplicitTreap::erase");
    auto [left, tail] = split_nodes(root_, position);
    auto [middle, right] = split_nodes(tail, 1);
    push(middle);
    T value = std::move(node(middle).value);
    release_node(middle);
    root_ = merge_nodes(left, right);
    return value;
  }

  T pop_front() {
    if (empty()) throw std::out_of_range("ImplicitTreap::pop_front");
    return erase(0);
  }

  T pop_back() {
    if (empty()) throw std::out_of_range("ImplicitTreap::pop_back");
    return erase(size() - 1);
  }

  std::vector<T> erase_range(size_type l, size_type r) {
    treap_detail::require_range(l, r, size(), "ImplicitTreap::erase_range");
    if (l == r) return {};
    auto [left, tail] = split_nodes(root_, l);
    auto [middle, right] = split_nodes(tail, r - l);
    std::vector<T> removed = dump_subtree(middle);
    release_subtree(middle);
    root_ = merge_nodes(left, right);
    return removed;
  }

  void replace_range(size_type l, size_type r, std::vector<T> values) {
    treap_detail::require_range(l, r, size(),
                                "ImplicitTreap::replace_range");
    auto [left, tail] = split_nodes(root_, l);
    auto [middle, right] = split_nodes(tail, r - l);
    release_subtree(middle);
    const int replacement = build_nodes(std::move(values));
    root_ = merge_nodes(merge_nodes(left, replacement), right);
  }

  [[nodiscard]] std::vector<T> slice(size_type l, size_type r) {
    treap_detail::require_range(l, r, size(), "ImplicitTreap::slice");
    if (l == r) return {};
    auto [left, tail] = split_nodes(root_, l);
    auto [middle, right] = split_nodes(tail, r - l);
    std::vector<T> result = dump_subtree(middle);
    root_ = merge_nodes(merge_nodes(left, middle), right);
    return result;
  }

  [[nodiscard]] std::vector<T> to_vector() { return dump_subtree(root_); }

  aggregate_type all_query() const {
    return root_ == null ? policy_.identity() : node(root_).forward;
  }

  aggregate_type range_query(size_type l, size_type r) {
    treap_detail::require_range(l, r, size(), "ImplicitTreap::range_query");
    if (l == r) return policy_.identity();
    auto [left, tail] = split_nodes(root_, l);
    auto [middle, right] = split_nodes(tail, r - l);
    aggregate_type result = node(middle).forward;
    root_ = merge_nodes(merge_nodes(left, middle), right);
    return result;
  }

  void apply(size_type l, size_type r, const tag_type& tag) {
    treap_detail::require_range(l, r, size(), "ImplicitTreap::apply");
    if (l == r) return;
    auto [left, tail] = split_nodes(root_, l);
    auto [middle, right] = split_nodes(tail, r - l);
    apply_tag(middle, tag);
    root_ = merge_nodes(merge_nodes(left, middle), right);
  }

  void reverse(size_type l, size_type r) {
    treap_detail::require_range(l, r, size(), "ImplicitTreap::reverse");
    if (r - l <= 1) return;
    auto [left, tail] = split_nodes(root_, l);
    auto [middle, right] = split_nodes(tail, r - l);
    toggle_reverse(middle);
    root_ = merge_nodes(merge_nodes(left, middle), right);
  }

  void reverse_all() { toggle_reverse(root_); }

  // Rotates [l, r) so that m becomes its first position:
  // [l, m) [m, r) -> [m, r) [l, m).
  void rotate(size_type l, size_type m, size_type r) {
    treap_detail::require_range(l, m, size(), "ImplicitTreap::rotate");
    treap_detail::require_range(m, r, size(), "ImplicitTreap::rotate");
    if (l == m || m == r) return;
    auto [a, tail1] = split_nodes(root_, l);
    auto [b, tail2] = split_nodes(tail1, m - l);
    auto [c, d] = split_nodes(tail2, r - m);
    root_ = merge_nodes(merge_nodes(merge_nodes(a, c), b), d);
  }

  // Cuts [l, r) and inserts it immediately before the original boundary pos.
  // If pos is inside [l, r], nothing changes.
  void move_range(size_type l, size_type r, size_type pos) {
    treap_detail::require_range(l, r, size(), "ImplicitTreap::move_range");
    treap_detail::require_insert_position(pos, size(),
                                          "ImplicitTreap::move_range");
    if (l == r || (l <= pos && pos <= r)) return;
    if (pos < l) {
      rotate(pos, l, r);
    } else {
      rotate(l, r, pos);
    }
  }

  // Swaps two disjoint ranges. Their lengths may differ.
  void swap_ranges(size_type l1, size_type r1, size_type l2, size_type r2) {
    treap_detail::require_range(l1, r1, size(), "ImplicitTreap::swap_ranges");
    treap_detail::require_range(l2, r2, size(), "ImplicitTreap::swap_ranges");
    if (l2 < l1) {
      std::swap(l1, l2);
      std::swap(r1, r2);
    }
    if (r1 > l2)
      throw std::invalid_argument("ImplicitTreap::swap_ranges: overlap");
    if (l1 == r1 && l2 == r2) return;

    auto [a, tail1] = split_nodes(root_, l1);
    auto [b, tail2] = split_nodes(tail1, r1 - l1);
    auto [c, tail3] = split_nodes(tail2, l2 - r1);
    auto [d, e] = split_nodes(tail3, r2 - l2);
    root_ = merge_nodes(
        merge_nodes(merge_nodes(merge_nodes(a, d), c), b), e);
  }

  template <class CompareValue = std::less<T>>
  void sort_range(size_type l, size_type r,
                  CompareValue comp = CompareValue{}) {
    std::vector<T> values = erase_range(l, r);
    std::sort(values.begin(), values.end(), std::move(comp));
    insert_range(l, std::move(values));
  }

  template <class CompareValue = std::less<T>>
  void stable_sort_range(size_type l, size_type r,
                         CompareValue comp = CompareValue{}) {
    std::vector<T> values = erase_range(l, r);
    std::stable_sort(values.begin(), values.end(), std::move(comp));
    insert_range(l, std::move(values));
  }

  template <class CompareValue = std::less<T>>
  void sort(CompareValue comp = CompareValue{}) {
    sort_range(0, size(), std::move(comp));
  }

  // These O(log N) searches are valid only when the sequence is already sorted
  // according to comp. The explicit _sorted suffix prevents accidental misuse.
  template <class U, class CompareValue = std::less<>>
  size_type lower_bound_sorted(const U& value,
                               CompareValue comp = CompareValue{}) {
    size_type offset = 0;
    size_type answer = size();
    int t = root_;
    while (t != null) {
      push(t);
      const size_type left_size = subtree_size(node(t).left);
      if (!std::invoke(comp, node(t).value, value)) {
        answer = offset + left_size;
        t = node(t).left;
      } else {
        offset += left_size + 1;
        t = node(t).right;
      }
    }
    return answer;
  }

  template <class U, class CompareValue = std::less<>>
  size_type upper_bound_sorted(const U& value,
                               CompareValue comp = CompareValue{}) {
    size_type offset = 0;
    size_type answer = size();
    int t = root_;
    while (t != null) {
      push(t);
      const size_type left_size = subtree_size(node(t).left);
      if (std::invoke(comp, value, node(t).value)) {
        answer = offset + left_size;
        t = node(t).left;
      } else {
        offset += left_size + 1;
        t = node(t).right;
      }
    }
    return answer;
  }

  template <class U, class CompareValue = std::less<>>
  std::pair<size_type, size_type> equal_range_sorted(
      const U& value, CompareValue comp = CompareValue{}) {
    const size_type lower = lower_bound_sorted(value, comp);
    const size_type upper = upper_bound_sorted(value, comp);
    return {lower, upper};
  }

  template <class U, class CompareValue = std::less<>>
  bool binary_search_sorted(const U& value,
                            CompareValue comp = CompareValue{}) {
    const size_type position = lower_bound_sorted(value, comp);
    if (position == size()) return false;
    const T found = get(position);
    return !std::invoke(comp, found, value) &&
           !std::invoke(comp, value, found);
  }

  // ACL-like max_right/min_left. pred(identity()) must be true.
  // max_right(l, pred): largest r such that pred(fold[l, r)) is true.
  template <class Predicate>
  size_type max_right(size_type l, Predicate&& pred) {
    treap_detail::require_insert_position(l, size(),
                                          "ImplicitTreap::max_right");
    aggregate_type accumulator = policy_.identity();
    if (!std::invoke(pred, accumulator))
      throw std::invalid_argument("ImplicitTreap::max_right: pred(e) false");
    auto [left, right] = split_nodes(root_, l);
    const size_type length = max_prefix(right, accumulator, pred);
    root_ = merge_nodes(left, right);
    return l + length;
  }

  // min_left(r, pred): smallest l such that pred(fold[l, r)) is true.
  template <class Predicate>
  size_type min_left(size_type r, Predicate&& pred) {
    treap_detail::require_insert_position(r, size(),
                                          "ImplicitTreap::min_left");
    aggregate_type accumulator = policy_.identity();
    if (!std::invoke(pred, accumulator))
      throw std::invalid_argument("ImplicitTreap::min_left: pred(e) false");
    auto [left, right] = split_nodes(root_, r);
    const size_type length = max_suffix(left, accumulator, pred);
    root_ = merge_nodes(left, right);
    return r - length;
  }

  [[nodiscard]] size_type height() const {
    if (root_ == null) return 0;
    size_type answer = 0;
    std::vector<std::pair<int, size_type>> stack{{root_, 1}};
    while (!stack.empty()) {
      const auto [t, depth] = stack.back();
      stack.pop_back();
      answer = std::max(answer, depth);
      if (node(t).left != null) stack.emplace_back(node(t).left, depth + 1);
      if (node(t).right != null)
        stack.emplace_back(node(t).right, depth + 1);
    }
    return answer;
  }

  // O(N) debug check: no cycle/duplicate live node, heap order and sizes.
  [[nodiscard]] bool validate_structure() const {
    std::vector<unsigned char> seen(nodes_.size(), 0);
    const auto result = validate_rec(root_, seen);
    return result.ok && result.size == size();
  }

  [[nodiscard]] const Policy& policy() const noexcept { return policy_; }

 protected:
  static constexpr int null = -1;

 private:
  struct Node {
    T value;
    aggregate_type forward;
    aggregate_type backward;
    std::optional<tag_type> lazy;
    std::uint64_t priority;
    int left = null;
    int right = null;
    size_type subtree_size = 1;
    bool reversed = false;

    Node(T v, std::uint64_t p, const Policy& policy)
        : value(std::move(v)),
          forward(policy.make_aggregate(value)),
          backward(forward),
          priority(p) {}
  };

  struct ValidationResult {
    bool ok;
    size_type size;
  };

  Policy policy_;
  treap_detail::SplitMix64 rng_;
  std::vector<Node> nodes_;
  std::vector<int> free_;
  int root_ = null;

  Node& node(int index) { return nodes_[static_cast<size_type>(index)]; }
  const Node& node(int index) const {
    return nodes_[static_cast<size_type>(index)];
  }

  [[nodiscard]] size_type subtree_size(int t) const noexcept {
    return t == null ? 0 : node(t).subtree_size;
  }

  aggregate_type forward_aggregate(int t) const {
    return t == null ? policy_.identity() : node(t).forward;
  }

  aggregate_type backward_aggregate(int t) const {
    return t == null ? policy_.identity() : node(t).backward;
  }

  int make_node(T value) {
    const std::uint64_t priority = rng_();
    if (free_.empty()) {
      const int index = static_cast<int>(nodes_.size());
      nodes_.emplace_back(std::move(value), priority, policy_);
      return index;
    }
    const int index = free_.back();
    free_.pop_back();
    node(index) = Node(std::move(value), priority, policy_);
    return index;
  }

  void release_node(int t) { free_.push_back(t); }

  void release_subtree(int root) {
    if (root == null) return;
    std::vector<int> stack{root};
    while (!stack.empty()) {
      const int t = stack.back();
      stack.pop_back();
      if (node(t).left != null) stack.push_back(node(t).left);
      if (node(t).right != null) stack.push_back(node(t).right);
      release_node(t);
    }
  }

  void pull(int t) {
    node(t).subtree_size =
        subtree_size(node(t).left) + 1 + subtree_size(node(t).right);
    const aggregate_type self = policy_.make_aggregate(node(t).value);
    node(t).forward = policy_.combine(
        policy_.combine(forward_aggregate(node(t).left), self),
        forward_aggregate(node(t).right));
    node(t).backward = policy_.combine(
        policy_.combine(backward_aggregate(node(t).right), self),
        backward_aggregate(node(t).left));
  }

  void apply_tag(int t, const tag_type& tag) {
    if (t == null) return;
    policy_.apply_value(node(t).value, tag);
    policy_.apply_aggregate(node(t).forward, tag, node(t).subtree_size);
    policy_.apply_aggregate(node(t).backward, tag, node(t).subtree_size);
    if (node(t).lazy) {
      node(t).lazy = policy_.compose(tag, *node(t).lazy);
    } else {
      node(t).lazy = tag;
    }
  }

  void toggle_reverse(int t) {
    if (t == null) return;
    std::swap(node(t).left, node(t).right);
    std::swap(node(t).forward, node(t).backward);
    node(t).reversed = !node(t).reversed;
  }

  void push(int t) {
    if (t == null) return;
    if (node(t).reversed) {
      toggle_reverse(node(t).left);
      toggle_reverse(node(t).right);
      node(t).reversed = false;
    }
    if (node(t).lazy) {
      const tag_type tag = *node(t).lazy;
      apply_tag(node(t).left, tag);
      apply_tag(node(t).right, tag);
      node(t).lazy.reset();
    }
  }

  std::pair<int, int> split_nodes(int t, size_type left_count) {
    if (t == null) return {null, null};
    push(t);
    const size_type left_size = subtree_size(node(t).left);
    if (left_count <= left_size) {
      auto [left, middle] = split_nodes(node(t).left, left_count);
      node(t).left = middle;
      pull(t);
      return {left, t};
    }
    auto [middle, right] =
        split_nodes(node(t).right, left_count - left_size - 1);
    node(t).right = middle;
    pull(t);
    return {t, right};
  }

  int merge_nodes(int left, int right) {
    if (left == null) return right;
    if (right == null) return left;
    if (node(left).priority >= node(right).priority) {
      push(left);
      node(left).right = merge_nodes(node(left).right, right);
      pull(left);
      return left;
    }
    push(right);
    node(right).left = merge_nodes(left, node(right).left);
    pull(right);
    return right;
  }

  int build_nodes(std::vector<T> values) {
    if (values.empty()) return null;
    std::vector<int> stack;
    stack.reserve(values.size());
    for (T& value : values) {
      const int current = make_node(std::move(value));
      int last = null;
      while (!stack.empty() &&
             node(stack.back()).priority < node(current).priority) {
        last = stack.back();
        stack.pop_back();
      }
      node(current).left = last;
      if (!stack.empty()) node(stack.back()).right = current;
      stack.push_back(current);
    }
    const int root = stack.front();
    recalculate_subtree(root);
    return root;
  }

  void recalculate_subtree(int root) {
    if (root == null) return;
    std::vector<std::pair<int, bool>> stack{{root, false}};
    while (!stack.empty()) {
      const auto [t, visited] = stack.back();
      stack.pop_back();
      if (visited) {
        pull(t);
      } else {
        stack.emplace_back(t, true);
        if (node(t).right != null) stack.emplace_back(node(t).right, false);
        if (node(t).left != null) stack.emplace_back(node(t).left, false);
      }
    }
  }

  T get_rec(int t, size_type position) {
    push(t);
    const size_type left_size = subtree_size(node(t).left);
    if (position < left_size) return get_rec(node(t).left, position);
    if (position == left_size) return node(t).value;
    return get_rec(node(t).right, position - left_size - 1);
  }

  int set_rec(int t, size_type position, T& value) {
    push(t);
    const size_type left_size = subtree_size(node(t).left);
    if (position < left_size) {
      node(t).left = set_rec(node(t).left, position, value);
    } else if (position == left_size) {
      node(t).value = std::move(value);
    } else {
      node(t).right =
          set_rec(node(t).right, position - left_size - 1, value);
    }
    pull(t);
    return t;
  }

  template <class Function>
  int modify_rec(int t, size_type position, Function& function) {
    push(t);
    const size_type left_size = subtree_size(node(t).left);
    if (position < left_size) {
      node(t).left = modify_rec(node(t).left, position, function);
    } else if (position == left_size) {
      using result_type = std::invoke_result_t<Function, T&>;
      if constexpr (std::is_void_v<result_type>) {
        std::invoke(function, node(t).value);
      } else {
        node(t).value = std::invoke(function, node(t).value);
      }
    } else {
      node(t).right =
          modify_rec(node(t).right, position - left_size - 1, function);
    }
    pull(t);
    return t;
  }

  std::vector<T> dump_subtree(int root) {
    std::vector<T> result;
    result.reserve(subtree_size(root));
    std::vector<int> stack;
    int current = root;
    while (current != null || !stack.empty()) {
      while (current != null) {
        push(current);
        stack.push_back(current);
        current = node(current).left;
      }
      current = stack.back();
      stack.pop_back();
      result.push_back(node(current).value);
      current = node(current).right;
    }
    return result;
  }

  template <class Predicate>
  size_type max_prefix(int t, aggregate_type& accumulator,
                       Predicate& pred) {
    if (t == null) return 0;
    push(t);
    const int left = node(t).left;
    const aggregate_type with_left =
        policy_.combine(accumulator, forward_aggregate(left));
    if (!std::invoke(pred, with_left)) {
      return max_prefix(left, accumulator, pred);
    }
    accumulator = with_left;
    const aggregate_type with_self = policy_.combine(
        accumulator, policy_.make_aggregate(node(t).value));
    if (!std::invoke(pred, with_self)) return subtree_size(left);
    accumulator = with_self;
    return subtree_size(left) + 1 +
           max_prefix(node(t).right, accumulator, pred);
  }

  template <class Predicate>
  size_type max_suffix(int t, aggregate_type& accumulator,
                       Predicate& pred) {
    if (t == null) return 0;
    push(t);
    const int right = node(t).right;
    const aggregate_type with_right =
        policy_.combine(forward_aggregate(right), accumulator);
    if (!std::invoke(pred, with_right)) {
      return max_suffix(right, accumulator, pred);
    }
    accumulator = with_right;
    const aggregate_type with_self = policy_.combine(
        policy_.make_aggregate(node(t).value), accumulator);
    if (!std::invoke(pred, with_self)) return subtree_size(right);
    accumulator = with_self;
    return subtree_size(right) + 1 +
           max_suffix(node(t).left, accumulator, pred);
  }

  ValidationResult validate_rec(int t,
                                std::vector<unsigned char>& seen) const {
    if (t == null) return {true, 0};
    const size_type index = static_cast<size_type>(t);
    if (index >= nodes_.size() || seen[index]) return {false, 0};
    seen[index] = 1;
    const Node& current = node(t);
    if (current.left != null &&
        node(current.left).priority > current.priority) {
      return {false, 0};
    }
    if (current.right != null &&
        node(current.right).priority > current.priority) {
      return {false, 0};
    }
    const auto left = validate_rec(current.left, seen);
    const auto right = validate_rec(current.right, seen);
    const size_type expected = left.size + 1 + right.size;
    return {left.ok && right.ok && expected == current.subtree_size, expected};
  }
};

template <class T>
using TreapVector = ImplicitTreap<T, PlainSequencePolicy<T>>;

template <class T, class Op>
using FoldableTreapVector = ImplicitTreap<T, FoldSequencePolicy<T, Op>>;

template <class T>
class AffineSumTreapVector : public ImplicitTreap<T, AffineSumPolicy<T>> {
  using Base = ImplicitTreap<T, AffineSumPolicy<T>>;

 public:
  using Base::Base;
  using size_type = typename Base::size_type;

  void range_affine(size_type l, size_type r, const T& multiply,
                    const T& add) {
    Base::apply(l, r, AffineTag<T>{multiply, add});
  }
  void range_add(size_type l, size_type r, const T& value) {
    range_affine(l, r, T{1}, value);
  }
  void range_multiply(size_type l, size_type r, const T& value) {
    range_affine(l, r, value, T{});
  }
  void range_assign(size_type l, size_type r, const T& value) {
    range_affine(l, r, T{}, value);
  }
  T range_sum(size_type l, size_type r) {
    return Base::range_query(l, r).sum;
  }
  T all_sum() const { return Base::all_query().sum; }
};

template <class T>
class NumericTreapVector : public ImplicitTreap<T, NumericAffinePolicy<T>> {
  using Base = ImplicitTreap<T, NumericAffinePolicy<T>>;

 public:
  using Base::Base;
  using size_type = typename Base::size_type;

  void range_affine(size_type l, size_type r, const T& multiply,
                    const T& add) {
    Base::apply(l, r, AffineTag<T>{multiply, add});
  }
  void range_add(size_type l, size_type r, const T& value) {
    range_affine(l, r, T{1}, value);
  }
  void range_multiply(size_type l, size_type r, const T& value) {
    range_affine(l, r, value, T{});
  }
  void range_assign(size_type l, size_type r, const T& value) {
    range_affine(l, r, T{}, value);
  }
  T range_sum(size_type l, size_type r) {
    return Base::range_query(l, r).sum;
  }
  T range_min(size_type l, size_type r) {
    const auto aggregate = Base::range_query(l, r);
    if (aggregate.count == 0)
      throw std::invalid_argument("NumericTreapVector::range_min: empty");
    return aggregate.minimum;
  }
  T range_max(size_type l, size_type r) {
    const auto aggregate = Base::range_query(l, r);
    if (aggregate.count == 0)
      throw std::invalid_argument("NumericTreapVector::range_max: empty");
    return aggregate.maximum;
  }
  T all_sum() const { return Base::all_query().sum; }
  T all_min() const {
    if (Base::empty())
      throw std::invalid_argument("NumericTreapVector::all_min: empty");
    return Base::all_query().minimum;
  }
  T all_max() const {
    if (Base::empty())
      throw std::invalid_argument("NumericTreapVector::all_max: empty");
    return Base::all_query().maximum;
  }
};

/*

*/

}  // namespace cp


struct Concat {
  string operator()(const string& a, const string& b) const { return a + b; }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  // -------------------------------------------------------------------------
  // 1. 値順: multiset + k 番目 + 順位
  // -------------------------------------------------------------------------
  cp::TreapMultiSet<ll> ordered{50, 20, 50, 10, 80};

  ordered.insert(30);                // {10,20,30,50,50,80}
  ordered.erase_one(50);             // 50 を1個だけ削除
  cout << ordered.kth(2) << '\n';    // 0-indexed 2番目 = 30
  cout << ordered.order_of_key(50)   // 50未満の個数 = 3
       << '\n';

  // 更新可能な中央値。pair<score, id> にすると、同点でも個人を特定できる。
  vector<ll> score{40, 80, 40, 10, 60};
  cp::TreapSet<pair<ll, int>> students;
  for (int i = 0; i < static_cast<int>(score.size()); ++i) {
    students.insert({score[i], i});
  }

  const int id = 1;
  students.erase_one({score[id], id});
  score[id] = 30;
  students.insert({score[id], id});

  const auto [median_score, median_id] =
      students.kth((students.size() - 1) / 2);
  cout << median_score << ' ' << median_id + 1 << '\n';

  // map版もk番目・順位に対応する。
  cp::TreapMap<int, string> names{{20, "Bob"}, {10, "Alice"}};
  names.insert_or_assign(30, "Carol");
  names.modify(20, [](string& name) { name += " Jr."; });
  cout << names.kth(1).second << '\n';

  // -------------------------------------------------------------------------
  // 3. 位置順: vector の途中挿入・削除・反転・移動
  // -------------------------------------------------------------------------
  cp::TreapVector<int> sequence{1, 2, 3, 4, 5};
  sequence.insert(2, 99);       // 1 2 99 3 4 5
  sequence.reverse(1, 5);       // 1 4 3 99 2 5
  sequence.move_range(1, 3, 6); // [4,3] を末尾境界の直前へ
  sequence.set(0, 100);

  for (int x : sequence.to_vector()) cout << x << ' ';
  cout << '\n';

  // 整列済みのときだけ使う O(log N) の二分探索。
  sequence.sort();
  cout << sequence.lower_bound_sorted(5) << '\n';

  // -------------------------------------------------------------------------
  // 4. 数値列: 区間一次変換・和・最小・最大・反転
  // -------------------------------------------------------------------------
  cp::NumericTreapVector<ll> values{1, 2, 3, 4, 5};
  values.range_add(1, 4, 10);       // 1 12 13 14 5
  values.range_multiply(0, 2, -1); // -1 -12 13 14 5
  values.range_assign(4, 5, 100);  // -1 -12 13 14 100
  values.range_affine(2, 4, 2, 1); // x <- 2x+1

  cout << values.range_sum(0, values.size()) << '\n';
  cout << values.range_min(0, values.size()) << '\n';
  cout << values.range_max(0, values.size()) << '\n';

  // -------------------------------------------------------------------------
  // 5. 任意モノイド: 文字列連結（非可換）+ 区間反転
  // -------------------------------------------------------------------------
  using StringPolicy = cp::FoldSequencePolicy<string, Concat>;
  cp::ImplicitTreap<string, StringPolicy> text(
      vector<string>{"A", "B", "C", "D"}, StringPolicy{"", Concat{}});

  text.reverse(1, 4);                 // A D C B
  cout << text.range_query(0, 4)      // "ADCB"
       << '\n';
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
