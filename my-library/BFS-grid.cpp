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

//◯問題
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);

    int h, w; cin >> h >> w;
    int sx, sy, gx, gy; cin >> sx >> sy >> gx >> gy;
    --sx; --sy; --gx; --gy;

    vector<string> g(h);
    rep(i,0,h) cin >> g[i];
    // rep(i,0,h) {
    //     rep(j,0,w) {
    //         if(g[i][j] == 'S') {
    //             sx = i;
    //             sy = j;
    //         }else if(g[i][j] == 'G') {
    //             gx = i;
    //             gy = j;
    //         }
    //     }
    // }

    //上下左右移動用
    const int dx[4] = {1, 0, -1, 0};
    const int dy[4] = {0, 1, 0, -1};

    vector<vector<ll>> dist(h, vector<ll>(w, -1));
    vector<vector<pair<int,int>>> parent(h, vector<pair<int,int>>(w, {-1,-1}));
    queue<pair<int,int>> q;

    dist[sx][sy] = 0;
    parent[sx][sy] = {-2,-2}; // 始点マーカー（別に無くてもOK）
    q.push({sx, sy});

    while(!q.empty()){
        auto [x, y] = q.front();
        q.pop();

        rep(dir,0,4){
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue; // 範囲外
            if(g[nx][ny] == '#') continue; // 壁
            if(dist[nx][ny] != -1) continue; // 既訪問

            dist[nx][ny] = dist[x][y] + 1;
            parent[nx][ny] = {x, y};
            q.push({nx, ny});
        }
    }
    //到達できなかったら-1を出力
    cout << dist[gx][gy] << endl;

    // ======= 経路復元したいとき=======
    /*
    if(dist[gx][gy] != -1){
        vector<pair<int,int>> path;
        pair<int,int> cur = {gx, gy};
        while(!(cur.fi == sx && cur.se == sy)){
            path.pb(cur);
            cur = parent[cur.fi][cur.se];
        }
        path.pb({sx, sy});
        reverse(nall(path));

        // 例：経路の座標を出力（1-indexに戻す）
        for(auto [x,y] : path){
            cout << x+1 << " " << y+1 << "\n";
        }
    }
    */
   //From chanini library
    return 0;
}
