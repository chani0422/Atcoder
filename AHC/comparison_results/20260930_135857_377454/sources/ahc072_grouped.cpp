#include <bits/stdc++.h>
using namespace std;

// AHC072: 同色のスライムを最大7匹にまとめて、最短経路で帰巣させる。
// まだ処理していない別色の1匹がいる床にも着地できるよう、1枠空ける。

struct Move {
    int r, c, k;
    char dir;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, K;
    if (!(cin >> N >> K)) return 0;
    vector<string> C(N);
    for (auto& row : C) cin >> row;

    constexpr int MAX_OPS = 100000;
    constexpr int GROUP_LIMIT = 7;
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};
    const string directions = "UDLR";

    vector<pair<int, int>> nest(K, {-1, -1});
    vector<vector<int>> nest_color(N, vector<int>(N, -1));
    // 下から上の順。back() が一番上。
    vector<vector<vector<int>>> tower(N, vector<vector<int>>(N));
    vector<array<int, 3>> initial_slimes;

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            char ch = C[r][c];
            if ('a' <= ch && ch <= 'l') {
                int color = ch - 'a';
                tower[r][c].push_back(color);
                initial_slimes.push_back({r, c, color});
            } else if ('A' <= ch && ch <= 'L') {
                int color = ch - 'A';
                nest[color] = {r, c};
                nest_color[r][c] = color;
            }
        }
    }

    auto inside = [&](int r, int c) {
        return 0 <= r && r < N && 0 <= c && c < N;
    };

    vector<vector<vector<int>>> dist(
        K, vector<vector<int>>(N, vector<int>(N, -1)));
    for (int color = 0; color < K; ++color) {
        auto [sr, sc] = nest[color];
        queue<pair<int, int>> q;
        dist[color][sr][sc] = 0;
        q.push({sr, sc});
        while (!q.empty()) {
            auto [r, c] = q.front();
            q.pop();
            for (int d = 0; d < 4; ++d) {
                int nr = r + dr[d], nc = c + dc[d];
                if (!inside(nr, nc) || C[nr][nc] == '#') continue;
                if (dist[color][nr][nc] != -1) continue;
                dist[color][nr][nc] = dist[color][r][c] + 1;
                q.push({nr, nc});
            }
        }
    }

    long long baseline_T = 0;
    for (auto [r, c, color] : initial_slimes) {
        assert(dist[color][r][c] >= 0);
        baseline_T += dist[color][r][c];
    }

    vector<Move> moves;
    moves.reserve(static_cast<size_t>(min<long long>(baseline_T, MAX_OPS)));
    int remaining = static_cast<int>(initial_slimes.size());
    int group_moves = 0;
    int flushes = 0;
    int max_group = 0;

    auto return_home = [&](int r, int c) {
        int color = nest_color[r][c];
        if (color == -1) return;
        auto& t = tower[r][c];
        while (!t.empty() && t.back() == color) {
            t.pop_back();
            --remaining;
        }
    };

    // 塔の上から、指定した色が何匹連続しているか数える。
    auto top_count = [&](int r, int c, int color) {
        const auto& t = tower[r][c];
        int count = 0;
        for (int i = static_cast<int>(t.size()) - 1; i >= 0; --i) {
            if (t[i] != color) break;
            ++count;
        }
        return count;
    };

    bool limit_reached = false;
    for (int color = 0; color < K && !limit_reached; ++color) {
        // 各色をすべて帰してから次の色へ進む。
        // これにより、別色は元の位置に1匹ずつ残るだけになる。
        vector<pair<int, int>> cells;
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (dist[color][r][c] > 0) cells.push_back({r, c});
            }
        }
        sort(cells.begin(), cells.end(), [&](auto a, auto b) {
            int da = dist[color][a.first][a.second];
            int db = dist[color][b.first][b.second];
            if (da != db) return da > db;
            return a < b;
        });

        // 現在の色の一団を、巣に1マス近い床へ全部送る。
        // 合流先の容量が足りなければ、その一団を先に送る。
        function<bool(int, int)> send_group = [&](int r, int c) -> bool {
            int amount = top_count(r, c, color);
            if (amount == 0) return true;
            if (moves.size() >= MAX_OPS) return false;
            assert(amount <= GROUP_LIMIT && dist[color][r][c] > 0);

            int best_dir = -1;
            int best_count = -1;
            bool best_fits = false;
            for (int d = 0; d < 4; ++d) {
                int nr = r + dr[d], nc = c + dc[d];
                if (!inside(nr, nc) || C[nr][nc] == '#') continue;
                if (dist[color][nr][nc] != dist[color][r][c] - 1) continue;
                int count = top_count(nr, nc, color);
                bool fits = count + amount <= GROUP_LIMIT;

                // そのまま着地できる床を優先。その中で同色が多い床へ合流。
                if (best_dir == -1 || (fits && !best_fits)
                    || (fits == best_fits && count > best_count)) {
                    best_dir = d;
                    best_count = count;
                    best_fits = fits;
                }
            }
            assert(best_dir != -1);
            int nr = r + dr[best_dir], nc = c + dc[best_dir];

            if (!best_fits) {
                ++flushes;
                if (!send_group(nr, nc)) return false;
            }
            if (moves.size() >= MAX_OPS) return false;

            auto& src = tower[r][c];
            auto& dst = tower[nr][nc];
            int h = static_cast<int>(src.size());
            int leave = h - amount;
            assert(0 <= leave && leave <= 1);
            assert(top_count(nr, nc, color) + amount <= GROUP_LIMIT);
            assert(static_cast<int>(dst.size()) + amount <= 8);

            moves.push_back({r, c, leave, directions[best_dir]});
            // 上側の一団を反転して着地。全員同色なので色の並びは同じ。
            vector<int> flying(src.begin() + leave, src.end());
            src.resize(static_cast<size_t>(leave));
            reverse(flying.begin(), flying.end());
            dst.insert(dst.end(), flying.begin(), flying.end());
            group_moves += (amount >= 2);
            max_group = max(max_group, amount);
            return_home(r, c);
            return_home(nr, nc);
            return true;
        };

        for (auto [r, c] : cells) {
            if (!send_group(r, c)) {
                limit_reached = true;
                break;
            }
        }
    }

    // 先頭にTは出さず、1行に1操作。飛距離はすべて1。
    for (auto m : moves) {
        cout << m.r << ' ' << m.c << ' ' << m.k << ' '
             << m.dir << " 1\n";
    }
    long long T = static_cast<long long>(moves.size());
    cerr << "slimes=" << initial_slimes.size()
         << " baseline_T=" << baseline_T << '\n';
    cerr << "T=" << T << " E=" << remaining
         << " S=" << T + 100000LL * remaining << '\n';
    cerr << "group_moves=" << group_moves << " max_group=" << max_group
         << " flushes=" << flushes << '\n';
    if (remaining == 0) cerr << "saved_steps=" << baseline_T - T << '\n';
    else cerr << "Operation limit reached; some slimes remain.\n";
    return 0;
}
