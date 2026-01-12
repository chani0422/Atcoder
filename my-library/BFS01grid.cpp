#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=a;i<b;i++)
#define rrep(i,a,b) for(int i=a;i>=b;i--)
#define pb(x) push_back(x)
#define popb() pop_back()
#define fi first
#define se second
#define yn(x) do { if(x) cout << "Yes\n"; else cout << "No\n"; } while(0)
#define nall(a) a.begin(),a.end()
#define rall(a) a.rbegin(),a.rend()
#define For(x) for(auto a : x)cout << a << " ";
using ll = long long; using ld = long double;
using ull = unsigned long long;
using Graph = vector<vector<ll>>;

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);

    int h, w; cin >> h >> w;
    int sx, sy, gx, gy; cin >> sx >> sy >> gx >> gy;
    --sx; --sy; --gx; --gy;

    vector<string> g(h);
    rep(i,0,h) cin >> g[i];

    // 徒歩（cost 0）用：上下左右
    const int dx[4] = {1, 0, -1, 0};
    const int dy[4] = {0, 1, 0, -1};

    const int INF = 1e9;
    vector<vector<ll>> dist(h, vector<ll>(w, INF));
    // vector<vector<pair<int,int>>> parent(h, vector<pair<int,int>>(w, {-1,-1}));

    deque<pair<int,int>> dq;

    dist[sx][sy] = 0;
    dq.push_front({sx, sy});

    while(!dq.empty()){
        auto [x, y] = dq.front();
        dq.pop_front();

        ll cur = dist[x][y];

        //cost 0
        rep(dir,0,4){
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue; //範囲外
            if(g[nx][ny] == '#') continue; //既訪問

            if(dist[nx][ny] > cur){
                dist[nx][ny] = cur;
                dq.push_front({nx, ny});
                // parent[nx][ny] = {x, y};
            }
        }

        //cost 1
        rep(i,-2,3){
            rep(j,-2,3){
                if(abs(i) == abs(j)) continue; //マンハッタン距離考慮用
                int nx = x + i;
                int ny = y + j;

                if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue; //範囲外
                if(g[nx][ny] == '#') continue; //既訪問

                if(dist[nx][ny] > cur + 1){
                    dist[nx][ny] = cur + 1;
                    dq.push_back({nx, ny});
                    // parent[nx][ny] = {x, y};
                }
            }
        }
    }

    if(dist[gx][gy] == INF) cout << -1 << "\n";
    else cout << dist[gx][gy] << "\n";

    return 0;
}
