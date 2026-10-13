#include <bits/stdc++.h>
using namespace std;

// launchpad版を予備解に残し、経路木の候補集合を発射台圧縮後に比較する。
// 色順序は部分集合DPで厳密に決め、各発射台集合に最適な候補木を選ぶ。
// AI/APIへの呼び出しや、外部でのコード書き換えは行わない。
using Clock = chrono::steady_clock;
constexpr int GROUP_LIMIT = 7;
constexpr int MAX_OPS = 100000;
constexpr double SEARCH_SECONDS = 1.20;
constexpr int MAX_ROUNDS = 512;
constexpr double TOTAL_SECONDS = 1.35;
constexpr double POOL_SECONDS = 1.55;
constexpr int ROUTE_BATCH = 64;
constexpr int MAX_ROUTE_TRIALS = 400000;
constexpr int MAX_ROUTE_CANDIDATES = 48;
constexpr int ROUTE_POOL_MARGIN = 8;
constexpr int SNAPSHOT_ACCEPTED_INTERVAL = 256;
const int DR[4] = {-1, 1, 0, 0};
const int DC[4] = {0, 0, -1, 1};
const string DIRECTIONS = "UDLR";

struct Random {
    uint64_t state;
    uint32_t next() {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return static_cast<uint32_t>(state >> 32);
    }
    double unit() { return static_cast<double>(next()) / 4294967296.0; }
};

struct Transfer {
    int from, dir, amount;
    int length = 1;
};

struct Plan {
    bool complete = false;
    vector<Transfer> moves;
};

struct ColorData {
    int goal = -1;
    int slime_count = 0;
    long long distance_sum = 0;
    vector<int> initial, distance, order;
    vector<pair<int, int>> levels;
    vector<double> attraction;
};

struct Planner {
    const vector<array<int, 4>>& neighbors;
    Clock::time_point deadline;
    Random rng;

    Plan make_plan(const ColorData& data, int mode, size_t bound) {
        const bool randomized = mode >= 0;
        vector<int> counts = data.initial;
        vector<int> order = data.order;
        vector<array<double, 4>> noise(counts.size());
        if (randomized) {
            for (auto [begin, end] : data.levels) {
                for (int i = end - 1; i > begin; --i) {
                    int j = begin + static_cast<int>(rng.next() % static_cast<uint32_t>(i - begin + 1));
                    swap(order[i], order[j]);
                }
            }
            for (auto& row : noise) for (auto& value : row) value = rng.unit();
        }
        const double count_weight[4] = {1024, 512, 128, 1024};
        const double attraction_weight[4] = {0, 512, 1024, 256};
        const double noise_weight[4] = {1, 512, 256, 2048};

        Plan result;
        result.moves.reserve(min(bound, static_cast<size_t>(data.distance_sum)));
        int checks = 0;
        auto send = [&](auto&& self, int v) -> bool {
            int amount = counts[v];
            if (amount == 0) return true;
            if (result.moves.size() >= bound) return false;
            if (randomized && (++checks % 128 == 0) && Clock::now() >= deadline) return false;
            assert(0 < amount && amount <= GROUP_LIMIT && v != data.goal);

            int best_dir = -1;
            bool best_fits = false;
            double best_score = -numeric_limits<double>::infinity();
            for (int d = 0; d < 4; ++d) {
                int u = neighbors[v][d];
                if (u == -1 || data.distance[u] != data.distance[v] - 1) continue;
                bool fits = counts[u] + amount <= GROUP_LIMIT;
                double score = static_cast<double>(counts[u]);
                if (randomized) {
                    score = count_weight[mode] * counts[u]
                          + attraction_weight[mode] * data.attraction[u]
                          + noise_weight[mode] * noise[v][d];
                }
                if (best_dir == -1 || (fits && !best_fits)
                    || (fits == best_fits && score > best_score)) {
                    best_dir = d;
                    best_fits = fits;
                    best_score = score;
                }
            }
            assert(best_dir != -1);
            int u = neighbors[v][best_dir];
            if (!best_fits && !self(self, u)) return false;
            if (result.moves.size() >= bound) return false;
            assert(counts[u] + amount <= GROUP_LIMIT);
            result.moves.push_back({v, best_dir, amount});
            counts[v] = 0;
            counts[u] += amount;
            if (u == data.goal) counts[u] = 0;
            return true;
        };
        for (int v : order) if (!send(send, v)) return result;
        result.complete = true;
        return result;
    }
};


// 各床から隣接床への「次の行き先」を決め、巣を根とする木にする。
// 最短経路でなくても、閉路を作らなければ必ず巣へ到達する。
struct RouteState {
    vector<int> parent, direction, rotation;
};

struct RouteCandidate {
    RouteState state;
    int route_cost = 0;
    uint64_t hash = 0;
};

struct RouteEvaluation {
    int cost = 0;
    vector<int> active;
};

struct RouteSearch {
    const vector<array<int, 4>>& neighbors;
    const ColorData& data;
    Random& rng;
    RouteState current, best_tree;
    RouteEvaluation evaluation;
    int best_cost = 0;
    int iterations = 0;
    int accepted = 0;
    int improvements = 0;
    int eligible_snapshots = 0;
    vector<RouteCandidate> candidate_pool;
    unordered_set<uint64_t> candidate_hashes;

    uint64_t state_hash(const RouteState& state) const {
        uint64_t value = 1469598103934665603ULL;
        for (int v = 0; v < static_cast<int>(state.parent.size()); ++v) {
            uint64_t token = static_cast<uint64_t>(state.parent[v] + 2)
                           | (static_cast<uint64_t>(state.rotation[v] + 1) << 16);
            value = (value ^ token) * 1099511628211ULL;
        }
        return value;
    }

    void remember_candidate(const RouteState& state, int cost, bool force = false) {
        if (!force && cost > best_cost + ROUTE_POOL_MARGIN) return;
        uint64_t hash = state_hash(state);
        if (candidate_hashes.count(hash)) return;
        ++eligible_snapshots;
        RouteCandidate candidate{state, cost, hash};
        if (static_cast<int>(candidate_pool.size()) < MAX_ROUTE_CANDIDATES) {
            candidate_hashes.insert(hash);
            candidate_pool.push_back(std::move(candidate));
            return;
        }

        int replace = -1;
        if (force) {
            replace = static_cast<int>(max_element(
                candidate_pool.begin(), candidate_pool.end(),
                [](const RouteCandidate& lhs, const RouteCandidate& rhs) {
                    return lhs.route_cost < rhs.route_cost;
                }) - candidate_pool.begin());
            if (candidate_pool[replace].route_cost < cost) return;
        } else {
            uint32_t slot = static_cast<uint32_t>(
                (hash ^ (static_cast<uint64_t>(eligible_snapshots) * 0x9e3779b97f4a7c15ULL))
                % static_cast<uint64_t>(eligible_snapshots));
            if (slot >= candidate_pool.size()) return;
            replace = static_cast<int>(slot);
        }
        candidate_hashes.erase(candidate_pool[replace].hash);
        candidate_hashes.insert(hash);
        candidate_pool[replace] = std::move(candidate);
    }

    RouteEvaluation evaluate(const RouteState& state, vector<Transfer>* moves = nullptr) const {
        vector<int> counts = data.initial;
        vector<int> flow = data.initial;
        RouteEvaluation result;
        result.active.reserve(data.order.size());
        // 満杯になる場合は親の同色グループを先に送り、置き場所を空ける。
        auto send = [&](auto&& self, int v) -> void {
            int amount = counts[v];
            if (amount == 0 || v == data.goal) return;
            int u = state.parent[v];
            assert(u >= 0 && amount <= GROUP_LIMIT);
            if (counts[u] + amount > GROUP_LIMIT) self(self, u);
            assert(counts[u] + amount <= GROUP_LIMIT);
            ++result.cost;
            if (moves) moves->push_back({v, state.direction[v], amount});
            counts[v] = 0;
            counts[u] += amount;
            if (u == data.goal) counts[u] = 0;
        };
        auto visit = [&](auto&& self, int v) -> void {
            for (int step = 0; step < 4; ++step) {
                int d = (state.rotation[v] + step) % 4;
                int u = neighbors[v][d];
                if (u == -1 || state.parent[u] != v) continue;
                self(self, u);
                flow[v] += flow[u];
            }
            if (v != data.goal) {
                if (flow[v] > 0) result.active.push_back(v);
                send(send, v);
            }
        };
        visit(visit, data.goal);
        assert(flow[data.goal] == data.slime_count);
        return result;
    }

    RouteSearch(const vector<array<int, 4>>& adjacency, const ColorData& color,
                const Plan& reference, Random& random)
        : neighbors(adjacency), data(color), rng(random) {
        int V = static_cast<int>(data.initial.size());
        current.parent.assign(V, -1);
        current.direction.assign(V, -1);
        current.rotation.resize(V);
        for (int v = 0; v < V; ++v) current.rotation[v] = static_cast<int>(rng.next() % 4);
        for (int v : data.order) {
            int choice = -1;
            double weight = -1;
            for (int d = 0; d < 4; ++d) {
                int u = neighbors[v][d];
                if (u != -1 && data.distance[u] == data.distance[v] - 1) {
                    double score = data.attraction[u] + rng.unit() * 0.1;
                    if (score > weight) { weight = score; choice = d; }
                }
            }
            assert(choice != -1);
            current.direction[v] = choice;
            current.parent[v] = neighbors[v][choice];
        }
        // 初期の木に、既存の探索解が選んだ合流先を取り込む。
        for (auto move : reference.moves) {
            current.direction[move.from] = move.dir;
            current.parent[move.from] = neighbors[move.from][move.dir];
        }
        evaluation = evaluate(current);
        best_tree = current;
        best_cost = evaluation.cost;
        remember_candidate(best_tree, best_cost, true);
    }

    // 固定した経路木を、マスごとの物理容量(7または8)で再評価する。
    // グループを途中で分割しないため、ある頂点の実効容量は巣までの
    // 経路上にある物理容量の最小値になる。
    Plan capacity_plan(const RouteState& tree,
                       const vector<int>& physical_capacity) const {
        int V = static_cast<int>(data.initial.size());
        vector<int> limit(V, -1);
        limit[data.goal] = 8;
        auto find_limit = [&](auto&& self, int v) -> int {
            if (limit[v] != -1) return limit[v];
            int u = tree.parent[v];
            assert(u >= 0);
            return limit[v] = min(physical_capacity[v], self(self, u));
        };
        for (int v : data.order) find_limit(find_limit, v);

        vector<int> counts = data.initial;
        vector<int> flow = data.initial;
        Plan result;
        result.moves.reserve(static_cast<size_t>(data.distance_sum));
        auto send = [&](auto&& self, int v) -> void {
            int amount = counts[v];
            if (amount == 0 || v == data.goal) return;
            int u = tree.parent[v];
            assert(u >= 0 && amount <= limit[v]);
            if (counts[u] + amount > limit[u]) self(self, u);
            assert(counts[u] + amount <= limit[u]);
            result.moves.push_back({v, tree.direction[v], amount, 1});
            counts[v] = 0;
            counts[u] += amount;
            if (u == data.goal) counts[u] = 0;
        };
        auto visit = [&](auto&& self, int v) -> void {
            for (int step = 0; step < 4; ++step) {
                int d = (tree.rotation[v] + step) % 4;
                int u = neighbors[v][d];
                if (u == -1 || tree.parent[u] != v) continue;
                self(self, u);
                flow[v] += flow[u];
            }
            if (v != data.goal) send(send, v);
        };
        visit(visit, data.goal);
        assert(flow[data.goal] == data.slime_count);
        result.complete = true;
        return result;
    }

    void finalize_pool() {
        remember_candidate(best_tree, best_cost, true);
        uint64_t best_hash = state_hash(best_tree);
        auto it = find_if(candidate_pool.begin(), candidate_pool.end(),
                          [&](const RouteCandidate& candidate) {
                              return candidate.hash == best_hash;
                          });
        assert(it != candidate_pool.end());
        iter_swap(candidate_pool.begin(), it);
        sort(candidate_pool.begin() + 1, candidate_pool.end(),
             [](const RouteCandidate& lhs, const RouteCandidate& rhs) {
                 if (lhs.route_cost != rhs.route_cost) return lhs.route_cost < rhs.route_cost;
                 return lhs.hash < rhs.hash;
             });
    }

    void publish(Plan& output) const {
        if (best_cost >= static_cast<int>(output.moves.size())) return;
        Plan candidate;
        candidate.moves.reserve(static_cast<size_t>(best_cost));
        auto measured = evaluate(best_tree, &candidate.moves);
        assert(measured.cost == best_cost);
        candidate.complete = true;
        output = std::move(candidate);
    }

    void step() {
        ++iterations;
        // 周期ごとに発見済みの最良の木へ戻し、温度を上げて探索する。
        int phase = (iterations - 1) % 2048;
        if (phase == 0 && iterations > 1) {
            current = best_tree;
            evaluation = evaluate(current);
        }
        if (evaluation.active.empty()) return;
        int v = evaluation.active[rng.next() % evaluation.active.size()];
        bool reorder = rng.next() % 5 == 0;
        int old_value;
        if (reorder) {
            // 子の処理順も変更する。親を選ぶと実際の合流順に作用する。
            v = current.parent[v];
            old_value = current.rotation[v];
            current.rotation[v] = (old_value + 1 + static_cast<int>(rng.next() % 3)) % 4;
        } else {
            int d = static_cast<int>(rng.next() % 4);
            int u = neighbors[v][d];
            if (u == -1 || u == current.parent[v]) return;
            // vの子孫を親にすると閉路になるため、その変更だけは除外する。
            for (int a = u; a != -1; a = current.parent[a]) if (a == v) return;
            old_value = current.direction[v];
            current.direction[v] = d;
            current.parent[v] = u;
        }
        RouteEvaluation candidate = evaluate(current);
        int delta = candidate.cost - evaluation.cost;
        double progress = static_cast<double>(phase) / 2047.0;
        double temperature = 2.0 * pow(0.05, progress);
        bool accept = delta <= 0 || rng.unit() < exp(-static_cast<double>(delta) / temperature);
        if (accept) {
            evaluation = std::move(candidate);
            ++accepted;
            if (evaluation.cost < best_cost) {
                best_cost = evaluation.cost;
                best_tree = current;
                ++improvements;
                remember_candidate(best_tree, best_cost, true);
            } else if (accepted % SNAPSHOT_ACCEPTED_INTERVAL == 0) {
                remember_candidate(current, evaluation.cost);
            }
        } else if (reorder) {
            current.rotation[v] = old_value;
        } else {
            current.direction[v] = old_value;
            current.parent[v] = neighbors[v][old_value];
        }
    }
};

// 連続する同一グループの直進2回を、発射台からの2マス移動1回に置換する。
// 中間マスの移動前後の状態が同一になる場合だけを扱うため、他の計画に影響しない。
Plan compress_with_launchpads(const Plan& source, int later_mask,
                              const vector<int>& initial_color,
                              const vector<array<int, 4>>& neighbors) {
    Plan result;
    result.complete = source.complete;
    result.moves.reserve(source.moves.size());
    for (size_t i = 0; i < source.moves.size();) {
        const Transfer& first = source.moves[i];
        bool compressed = false;
        if (i + 1 < source.moves.size() && first.length == 1) {
            int support_color = initial_color[first.from];
            int middle = neighbors[first.from][first.dir];
            const Transfer& second = source.moves[i + 1];
            if (support_color >= 0 && ((later_mask >> support_color) & 1)
                && middle != -1 && neighbors[middle][first.dir] != -1
                && second.length == 1 && second.from == middle
                && second.dir == first.dir && second.amount == first.amount) {
                result.moves.push_back({first.from, first.dir, first.amount, 2});
                i += 2;
                compressed = true;
            }
        }
        if (!compressed) {
            result.moves.push_back(first);
            ++i;
        }
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const auto started = Clock::now();

    int N, K;
    if (!(cin >> N >> K)) return 0;
    // K=12では後段の2^(K-1)通りの評価が支配的になるため、
    // 前段探索を少し短くして、必ず復元時間を残す。
    const double search_seconds = (K >= 12 ? 1.00 : SEARCH_SECONDS);
    const double route_seconds = (K >= 12 ? 1.20 : TOTAL_SECONDS);
    const auto deadline = started + chrono::duration_cast<Clock::duration>(
        chrono::duration<double>(search_seconds));
    vector<string> grid(N);
    for (auto& row : grid) cin >> row;
    const int V = N * N;
    vector<int> initial_color(V, -1), nest_color(V, -1), goals(K, -1);
    vector<array<int, 4>> neighbors(V);
    for (auto& row : neighbors) row.fill(-1);
    uint64_t seed = 1469598103934665603ULL;
    int initial_total = 0;
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            int v = r * N + c;
            char ch = grid[r][c];
            seed = (seed ^ static_cast<unsigned char>(ch)) * 1099511628211ULL;
            if ('a' <= ch && ch <= 'l') {
                initial_color[v] = ch - 'a';
                ++initial_total;
            } else if ('A' <= ch && ch <= 'L') {
                nest_color[v] = ch - 'A';
                goals[ch - 'A'] = v;
            }
            if (ch == '#') continue;
            for (int d = 0; d < 4; ++d) {
                int nr = r + DR[d], nc = c + DC[d];
                if (0 <= nr && nr < N && 0 <= nc && nc < N && grid[nr][nc] != '#') {
                    neighbors[v][d] = nr * N + nc;
                }
            }
        }
    }

    vector<ColorData> data(K);
    long long baseline_T = 0;
    for (int color = 0; color < K; ++color) {
        auto& a = data[color];
        a.goal = goals[color];
        a.initial.assign(V, 0);
        a.distance.assign(V, -1);
        a.attraction.assign(V, 0);
        queue<int> q;
        q.push(a.goal);
        a.distance[a.goal] = 0;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int u : neighbors[v]) {
                if (u == -1 || a.distance[u] != -1) continue;
                a.distance[u] = a.distance[v] + 1;
                q.push(u);
            }
        }
        for (int v = 0; v < V; ++v) {
            if (a.distance[v] > 0) a.order.push_back(v);
            if (initial_color[v] == color) {
                a.initial[v] = 1;
                a.attraction[v] = 1;
                ++a.slime_count;
                a.distance_sum += a.distance[v];
            }
        }
        sort(a.order.begin(), a.order.end(), [&](int v, int u) {
            if (a.distance[v] != a.distance[u]) return a.distance[v] > a.distance[u];
            return v < u;
        });
        for (int begin = 0; begin < static_cast<int>(a.order.size());) {
            int end = begin + 1;
            while (end < static_cast<int>(a.order.size())
                   && a.distance[a.order[end]] == a.distance[a.order[begin]]) ++end;
            a.levels.push_back({begin, end});
            begin = end;
        }
        // 同色の流れが集まりやすい床を、最短経路DAG上で見積もる。
        for (int v : a.order) {
            int choices = 0;
            for (int u : neighbors[v]) {
                if (u != -1 && a.distance[u] == a.distance[v] - 1) ++choices;
            }
            for (int u : neighbors[v]) {
                if (u != -1 && a.distance[u] == a.distance[v] - 1) {
                    a.attraction[u] += a.attraction[v] / choices;
                }
            }
        }
        for (double& value : a.attraction) value = log1p(value);
        baseline_T += a.distance_sum;
    }

    Planner planner{neighbors, deadline, Random{seed == 0 ? 1 : seed}};
    vector<Plan> best(K);
    long long grouped_T = 0;
    // 初期候補は、既存grouped版と同じ処理順・同じ合流先の選び方。
    for (int color = 0; color < K; ++color) {
        best[color] = planner.make_plan(data[color], -1, numeric_limits<size_t>::max());
        assert(best[color].complete);
        grouped_T += static_cast<long long>(best[color].moves.size());
    }

    int trials = 0, improvements = 0;
    bool time_up = false;
    for (int round = 0; round < MAX_ROUNDS && !time_up; ++round) {
        for (int color = 0; color < K; ++color) {
            if (Clock::now() >= deadline) { time_up = true; break; }
            if (data[color].slime_count <= 1) continue;
            ++trials;
            Plan candidate = planner.make_plan(data[color], round % 4, best[color].moves.size());
            if (candidate.complete && candidate.moves.size() < best[color].moves.size()) {
                best[color] = std::move(candidate);
                ++improvements;
            }
        }
    }


    long long search_T = 0;
    for (const auto& plan : best) search_T += static_cast<long long>(plan.moves.size());
    const auto route_deadline = started + chrono::duration_cast<Clock::duration>(
        chrono::duration<double>(route_seconds));
    Random route_rng{seed ^ 0x9e3779b97f4a7c15ULL};
    if (route_rng.state == 0) route_rng.state = 1;
    vector<unique_ptr<RouteSearch>> routes(K);
    for (int color = 0; color < K; ++color) {
        if (data[color].slime_count <= 1 || Clock::now() >= route_deadline) continue;
        routes[color] = make_unique<RouteSearch>(neighbors, data[color], best[color], route_rng);
        routes[color]->publish(best[color]);
    }
    int route_trials = 0;
    bool has_routes = any_of(routes.begin(), routes.end(), [](const auto& p) { return bool(p); });
    while (has_routes && route_trials < MAX_ROUTE_TRIALS && Clock::now() < route_deadline) {
        for (int color = 0; color < K; ++color) {
            if (!routes[color]) continue;
            for (int step = 0; step < ROUTE_BATCH; ++step) {
                if (Clock::now() >= route_deadline || route_trials >= MAX_ROUTE_TRIALS) break;
                routes[color]->step();
                ++route_trials;
            }
            routes[color]->publish(best[color]);
            if (Clock::now() >= route_deadline || route_trials >= MAX_ROUTE_TRIALS) break;
        }
    }
    int route_accepted = 0, route_improvements = 0, detour_moves = 0;
    int pool_states = 0;
    for (int color = 0; color < K; ++color) {
        if (routes[color]) {
            routes[color]->finalize_pool();
            routes[color]->publish(best[color]);
            route_accepted += routes[color]->accepted;
            route_improvements += routes[color]->improvements;
            pool_states += static_cast<int>(routes[color]->candidate_pool.size());
        }
        for (auto move : best[color].moves) {
            int u = neighbors[move.from][move.dir];
            detour_moves += data[color].distance[u] >= data[color].distance[move.from];
        }
    }

    long long routes_T = 0;
    for (const auto& plan : best) routes_T += static_cast<long long>(plan.moves.size());

    const int full_mask = (1 << K) - 1;
    const int INF_COST = numeric_limits<int>::max() / 4;

    // mask に残る初期スライムは高さ1の発射台になるため、そのマスでは
    // 同色グループが使える容量を7にする。ほかのマスは高さ8まで使える。
    vector<vector<int>> physical_capacity(1 << K, vector<int>(V, 8));
    for (int mask = 0; mask <= full_mask; ++mask) {
        for (int v = 0; v < V; ++v) {
            int support_color = initial_color[v];
            if (support_color >= 0 && ((mask >> support_color) & 1)) {
                physical_capacity[mask][v] = 7;
            }
        }
    }

    // order_cost[color][later_mask] は、その発射台集合に対する最小操作数。
    // selected_pool=-1 は従来計画、0以上は経路候補プールの添字を表す。
    vector<vector<int>> order_cost(K, vector<int>(1 << K, INF_COST));
    vector<vector<int>> selected_pool(K, vector<int>(1 << K, -1));
    for (int color = 0; color < K; ++color) {
        for (int candidate_mask = 0; candidate_mask <= full_mask; ++candidate_mask) {
            if ((candidate_mask >> color) & 1) continue;
            Plan candidate = compress_with_launchpads(best[color], candidate_mask,
                                                      initial_color, neighbors);
            order_cost[color][candidate_mask] = static_cast<int>(candidate.moves.size());
        }
    }

    auto solve_order = [&](const vector<vector<int>>& costs) {
        vector<int> value(1 << K, INF_COST), prev(1 << K, -1), chosen(1 << K, -1);
        value[0] = 0;
        for (int mask = 0; mask <= full_mask; ++mask) {
            if (value[mask] == INF_COST) continue;
            for (int color = 0; color < K; ++color) {
                if ((mask >> color) & 1) continue;
                int next_mask = mask | (1 << color);
                int later_mask = full_mask ^ next_mask;
                int candidate = value[mask] + costs[color][later_mask];
                if (candidate < value[next_mask]) {
                    value[next_mask] = candidate;
                    prev[next_mask] = mask;
                    chosen[next_mask] = color;
                }
            }
        }
        vector<int> order;
        for (int mask = full_mask; mask != 0; mask = prev[mask]) {
            assert(prev[mask] >= 0 && chosen[mask] >= 0);
            order.push_back(chosen[mask]);
        }
        reverse(order.begin(), order.end());
        return pair<int, vector<int>>(value[full_mask], std::move(order));
    };

    const auto pool_deadline = started + chrono::duration_cast<Clock::duration>(
        chrono::duration<double>(POOL_SECONDS));
    vector<vector<vector<unsigned char>>> pool_evaluated(K);
    vector<vector<unsigned char>> candidate_started(K);
    for (int color = 0; color < K; ++color) if (routes[color]) {
        int count = static_cast<int>(routes[color]->candidate_pool.size());
        pool_evaluated[color].assign(count, vector<unsigned char>(1 << K, 0));
        candidate_started[color].assign(count, 0);
    }

    int pool_evaluations = 0, pool_updates = 0, pool_candidates_tested = 0;
    auto evaluate_one = [&](int color, int pool_index, int candidate_mask) -> bool {
        assert(routes[color] && ((candidate_mask >> color) & 1) == 0);
        if (pool_evaluated[color][pool_index][candidate_mask]) return true;
        if (Clock::now() >= pool_deadline) return false;
        if (!candidate_started[color][pool_index]) {
            candidate_started[color][pool_index] = 1;
            ++pool_candidates_tested;
        }
        const RouteState& tree = routes[color]->candidate_pool[pool_index].state;
        Plan candidate = routes[color]->capacity_plan(
            tree, physical_capacity[candidate_mask]);
        candidate = compress_with_launchpads(candidate, candidate_mask,
                                             initial_color, neighbors);
        pool_evaluated[color][pool_index][candidate_mask] = 1;
        ++pool_evaluations;
        int cost = static_cast<int>(candidate.moves.size());
        if (cost < order_cost[color][candidate_mask]) {
            order_cost[color][candidate_mask] = cost;
            selected_pool[color][candidate_mask] = pool_index;
            ++pool_updates;
        }
        return true;
    };

    // 添字0は従来版の最良経路木。容量8の効果が大きい発射台の少ないmaskから、
    // 色を交互に評価する。時刻に達しても未評価maskには常に安全な従来計画がある。
    bool pool_time_up = false;
    for (int bits = 0; bits < K && !pool_time_up; ++bits) {
        for (int candidate_mask = 0; candidate_mask <= full_mask && !pool_time_up;
             ++candidate_mask) {
            if (__builtin_popcount(static_cast<unsigned>(candidate_mask)) != bits) continue;
            for (int color = 0; color < K; ++color) {
                if (!routes[color] || ((candidate_mask >> color) & 1)) continue;
                if (!evaluate_one(color, 0, candidate_mask)) {
                    pool_time_up = true;
                    break;
                }
            }
        }
    }
    const vector<vector<int>> launchpad_order_cost = order_cost;
    const int launchpad_reference_T = solve_order(launchpad_order_cost).first;

    // 追加候補は、まず現在の最適順で実際に使うK個のmaskに集中させる。
    // 改善で順序が変わる場合に備え、最大4回だけDPと重点評価を繰り返す。
    for (int round = 0; round < 4 && !pool_time_up; ++round) {
        vector<int> provisional_order = solve_order(order_cost).second;
        int remaining = full_mask;
        int updates_before = pool_updates;
        for (int color : provisional_order) {
            remaining ^= 1 << color;
            if (!routes[color]) continue;
            int count = static_cast<int>(routes[color]->candidate_pool.size());
            for (int pool_index = 1; pool_index < count; ++pool_index) {
                if (!evaluate_one(color, pool_index, remaining)) {
                    pool_time_up = true;
                    break;
                }
            }
            if (pool_time_up) break;
        }
        if (pool_updates == updates_before) break;
    }

    // 重点評価後に時間が残れば、低コスト候補から未評価maskへ探索を広げる。
    for (int pool_index = 1; pool_index < MAX_ROUTE_CANDIDATES && !pool_time_up;
         ++pool_index) {
        for (int bits = 0; bits < K && !pool_time_up; ++bits) {
            for (int candidate_mask = 0; candidate_mask <= full_mask && !pool_time_up;
                 ++candidate_mask) {
                if (__builtin_popcount(static_cast<unsigned>(candidate_mask)) != bits) continue;
                for (int color = 0; color < K; ++color) {
                    if (!routes[color] || ((candidate_mask >> color) & 1)
                        || pool_index >= static_cast<int>(routes[color]->candidate_pool.size())) {
                        continue;
                    }
                    if (!evaluate_one(color, pool_index, candidate_mask)) {
                        pool_time_up = true;
                        break;
                    }
                }
            }
        }
    }

    // dp[mask]: mask の色を処理済みにした時点までの最小操作数。
    // 次に色cを処理すると、full ^ (mask | (1<<c)) が発射台として残る。
    vector<int> dp(1 << K, INF_COST), previous(1 << K, -1), selected_color(1 << K, -1);
    dp[0] = 0;
    for (int mask = 0; mask <= full_mask; ++mask) {
        if (dp[mask] == INF_COST) continue;
        for (int color = 0; color < K; ++color) {
            if ((mask >> color) & 1) continue;
            int next_mask = mask | (1 << color);
            int candidate_mask = full_mask ^ next_mask;
            int candidate = dp[mask] + order_cost[color][candidate_mask];
            if (candidate < dp[next_mask]) {
                dp[next_mask] = candidate;
                previous[next_mask] = mask;
                selected_color[next_mask] = color;
            }
        }
    }
    vector<int> color_order;
    for (int mask = full_mask; mask != 0; mask = previous[mask]) {
        assert(previous[mask] >= 0 && selected_color[mask] >= 0);
        color_order.push_back(selected_color[mask]);
    }
    reverse(color_order.begin(), color_order.end());

    auto make_candidate = [&](int color, int later_mask) -> Plan {
        Plan chosen = compress_with_launchpads(best[color], later_mask,
                                               initial_color, neighbors);
        int pool_index = selected_pool[color][later_mask];
        if (pool_index >= 0) {
            Plan candidate = routes[color]->capacity_plan(
                routes[color]->candidate_pool[pool_index].state,
                physical_capacity[later_mask]);
            candidate = compress_with_launchpads(candidate, later_mask,
                                                 initial_color, neighbors);
            if (candidate.moves.size() < chosen.moves.size()) chosen = std::move(candidate);
        }
        assert(static_cast<int>(chosen.moves.size()) == order_cost[color][later_mask]);
        return chosen;
    };

    vector<Plan> launch_plan(K);
    int remaining_mask = full_mask;
    long long launchpad_T = 0;
    int pool_selected = 0;
    for (int color : color_order) {
        remaining_mask ^= 1 << color;
        pool_selected += selected_pool[color][remaining_mask] >= 1;
        launch_plan[color] = make_candidate(color, remaining_mask);
        launchpad_T += static_cast<long long>(launch_plan[color].moves.size());
    }
    assert(launchpad_T <= launchpad_reference_T);
    // 理論上はDP候補が必ずroutes以下だが、予備解を明示的に保持する。
    if (launchpad_T > routes_T) {
        color_order.resize(K);
        iota(color_order.begin(), color_order.end(), 0);
        launch_plan = best;
        launchpad_T = routes_T;
    }
    int jump_moves = 0, eight_groups = 0;
    for (int color : color_order) {
        for (const auto& move : launch_plan[color].moves) {
            jump_moves += move.length == 2;
            eight_groups += move.amount == 8;
        }
    }

    // 色別の計画を実盤面で再現し、下に残す匹数kを確定する。
    vector<vector<int>> tower(V);
    for (int v = 0; v < V; ++v) if (initial_color[v] != -1) tower[v].push_back(initial_color[v]);
    int remaining = initial_total;
    auto home = [&](int v) {
        if (nest_color[v] == -1) return;
        auto& stack = tower[v];
        while (!stack.empty() && stack.back() == nest_color[v]) {
            stack.pop_back();
            --remaining;
        }
    };
    struct Output { int v, leave, dir, length; };
    vector<Output> answer;
    answer.reserve(static_cast<size_t>(min<long long>(launchpad_T, MAX_OPS)));
    for (int color : color_order) {
        for (auto move : launch_plan[color].moves) {
            if (answer.size() >= MAX_OPS) break;
            int v = move.from, u = v;
            for (int step = 0; step < move.length; ++step) {
                u = neighbors[u][move.dir];
                assert(u != -1);
            }
            int leave = static_cast<int>(tower[v].size()) - move.amount;
            assert(leave >= 0 && 1 <= move.length && move.length <= leave + 1);
            assert(static_cast<int>(tower[u].size()) + move.amount <= 8);
            for (int j = leave; j < static_cast<int>(tower[v].size()); ++j) assert(tower[v][j] == color);
            answer.push_back({v, leave, move.dir, move.length});
            vector<int> flying(tower[v].begin() + leave, tower[v].end());
            tower[v].resize(static_cast<size_t>(leave));
            reverse(flying.begin(), flying.end());
            tower[u].insert(tower[u].end(), flying.begin(), flying.end());
            home(v);
            home(u);
        }
        if (answer.size() >= MAX_OPS) break;
    }
    for (auto op : answer) {
        cout << op.v / N << ' ' << op.v % N << ' ' << op.leave << ' '
             << DIRECTIONS[op.dir] << ' ' << op.length << '\n';
    }
    long long T = static_cast<long long>(answer.size());
    cerr << "slimes=" << initial_total << " baseline_T=" << baseline_T << " grouped_T=" << grouped_T << '\n';
    cerr << "T=" << T << " E=" << remaining << " S=" << T + 100000LL * remaining << '\n';
    cerr << "trials=" << trials << " accepted=" << improvements
         << " elapsed_ms=" << chrono::duration<double, milli>(Clock::now() - started).count() << '\n';
    cerr << "search_T=" << search_T << " route_trials=" << route_trials
         << " route_accepted=" << route_accepted << " route_improvements=" << route_improvements
         << " detour_moves=" << detour_moves << '\n';
    cerr << "pool_states=" << pool_states << " pool_candidates_tested=" << pool_candidates_tested
         << " pool_evaluations=" << pool_evaluations << " pool_updates=" << pool_updates
         << " pool_selected=" << pool_selected << " pool_time_up=" << pool_time_up << '\n';
    cerr << "routes_T=" << routes_T << " launchpad_reference_T=" << launchpad_reference_T
         << " pool_T=" << launchpad_T << " jumps=" << jump_moves
         << " groups_of_8=" << eight_groups << " order=";
    for (int color : color_order) cerr << char('a' + color);
    cerr << '\n';
    if (remaining == 0) {
        cerr << "saved_vs_routes=" << routes_T - T
             << " saved_vs_launchpad=" << launchpad_reference_T - T << '\n';
    }
    return 0;
}
