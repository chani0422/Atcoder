#include <bits/stdc++.h>
using namespace std;

// 既存の探索解を残し、遠回りも含む合流用の木を焼きなましで改善する。
// AI/APIへの呼び出しや、外部でのコード書き換えは行わない。
using Clock = chrono::steady_clock;
constexpr int GROUP_LIMIT = 7;
constexpr int MAX_OPS = 100000;
constexpr double SEARCH_SECONDS = 1.20;
constexpr int MAX_ROUNDS = 512;
constexpr double TOTAL_SECONDS = 1.35;
constexpr int ROUTE_BATCH = 64;
constexpr int MAX_ROUTE_TRIALS = 400000;
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
            }
        } else if (reorder) {
            current.rotation[v] = old_value;
        } else {
            current.direction[v] = old_value;
            current.parent[v] = neighbors[v][old_value];
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const auto started = Clock::now();
    const auto deadline = started + chrono::duration_cast<Clock::duration>(
        chrono::duration<double>(SEARCH_SECONDS));

    int N, K;
    if (!(cin >> N >> K)) return 0;
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
        chrono::duration<double>(TOTAL_SECONDS));
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
    for (int color = 0; color < K; ++color) {
        if (routes[color]) {
            route_accepted += routes[color]->accepted;
            route_improvements += routes[color]->improvements;
        }
        for (auto move : best[color].moves) {
            int u = neighbors[move.from][move.dir];
            detour_moves += data[color].distance[u] >= data[color].distance[move.from];
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
    struct Output { int v, leave, dir; };
    vector<Output> answer;
    answer.reserve(static_cast<size_t>(min<long long>(grouped_T, MAX_OPS)));
    for (int color = 0; color < K; ++color) {
        for (auto move : best[color].moves) {
            if (answer.size() >= MAX_OPS) break;
            int v = move.from, u = neighbors[v][move.dir];
            int leave = static_cast<int>(tower[v].size()) - move.amount;
            assert(leave >= 0 && static_cast<int>(tower[u].size()) + move.amount <= 8);
            for (int j = leave; j < static_cast<int>(tower[v].size()); ++j) assert(tower[v][j] == color);
            answer.push_back({v, leave, move.dir});
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
             << DIRECTIONS[op.dir] << " 1\n";
    }
    long long T = static_cast<long long>(answer.size());
    cerr << "slimes=" << initial_total << " baseline_T=" << baseline_T << " grouped_T=" << grouped_T << '\n';
    cerr << "T=" << T << " E=" << remaining << " S=" << T + 100000LL * remaining << '\n';
    cerr << "trials=" << trials << " accepted=" << improvements
         << " elapsed_ms=" << chrono::duration<double, milli>(Clock::now() - started).count() << '\n';
    cerr << "search_T=" << search_T << " route_trials=" << route_trials
         << " route_accepted=" << route_accepted << " route_improvements=" << route_improvements
         << " detour_moves=" << detour_moves << '\n';
    if (remaining == 0) cerr << "saved_vs_search=" << search_T - T << '\n';
    return 0;
}
