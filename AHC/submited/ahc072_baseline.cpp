#include <bits/stdc++.h>
using namespace std;

// AHC072: 1匹ずつ、BFSの最短経路で帰巣させる基本解。
// 標準出力には操作だけを出し、確認用の情報は標準エラー出力へ出す。

struct Slime {
    int r, c, color;
};

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
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};
    const string directions = "UDLR";

    vector<pair<int, int>> nest(K, {-1, -1});
    vector<vector<int>> nest_color(N, vector<int>(N, -1));

    // tower[r][c] は下から上の順。back() が一番上。
    vector<vector<vector<int>>> tower(N, vector<vector<int>>(N));
    vector<Slime> slimes;

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            char ch = C[r][c];
            if ('a' <= ch && ch <= 'l') {
                int color = ch - 'a';
                tower[r][c].push_back(color);
                slimes.push_back({r, c, color});
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

    // dist[color][r][c]: この床から、指定した色の巣までの最短距離。
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

    long long planned_T = 0;
    for (auto s : slimes) planned_T += dist[s.color][s.r][s.c];

    vector<Move> moves;
    moves.reserve(static_cast<size_t>(min<long long>(planned_T, MAX_OPS)));
    int remaining = static_cast<int>(slimes.size());

    // 巣では、上から同じ色が続く限り帰巣する。
    auto return_home = [&](int r, int c) {
        int color = nest_color[r][c];
        if (color == -1) return;
        auto& t = tower[r][c];
        while (!t.empty() && t.back() == color) {
            t.pop_back();
            --remaining;
        }
    };

    // 初期位置の順に処理。未処理のスライムは元の位置に残っている。
    for (auto s : slimes) {
        if (moves.size() >= MAX_OPS) break;
        int r = s.r, c = s.c;
        auto [gr, gc] = nest[s.color];

        while ((r != gr || c != gc) && moves.size() < MAX_OPS) {
            int d = -1;
            for (int nd = 0; nd < 4; ++nd) {
                int nr = r + dr[nd], nc = c + dc[nd];
                if (!inside(nr, nc) || C[nr][nc] == '#') continue;
                if (dist[s.color][nr][nc] == dist[s.color][r][c] - 1) {
                    d = nd;
                    break;
                }
            }
            assert(d != -1);

            int nr = r + dr[d], nc = c + dc[d];
            int h = static_cast<int>(tower[r][c].size());
            assert(h > 0 && tower[r][c].back() == s.color);
            assert(tower[nr][nc].size() + 1 <= 8);

            // 上の1匹だけを、隣の床へ動かす。k=h-1、飛距離は1。
            moves.push_back({r, c, h - 1, directions[d]});
            int color = tower[r][c].back();
            tower[r][c].pop_back();
            tower[nr][nc].push_back(color);

            // 飛ぶのは1匹なので、反転しても並びは変わらない。
            return_home(r, c);
            return_home(nr, nc);
            r = nr;
            c = nc;
        }
    }

    // 操作回数Tを先頭に出力してはいけない。1行に1操作だけ出力。
    for (auto m : moves) {
        cout << m.r << ' ' << m.c << ' ' << m.k << ' '
             << m.dir << " 1\n";
    }

    long long T = static_cast<long long>(moves.size());
    cerr << "slimes=" << slimes.size() << " planned_T=" << planned_T << '\n';
    cerr << "T=" << T << " E=" << remaining
         << " S=" << T + 100000LL * remaining << '\n';
    if (remaining != 0) cerr << "Operation limit reached; some slimes remain.\n";
    return 0;
}
