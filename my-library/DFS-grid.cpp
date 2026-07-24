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
int h, w;
vector<string> grid;
vector<vector<bool>> seen;
vector<vector<pair<int,int>>> parent;

//上下左右移動用
const int dx[4] = {1, 0, -1, 0};
const int dy[4] = {0, 1, 0, -1};
const int dx[8] = {1, 1, 1, 0, 0, -1, -1, -1};
const int dy[8] = {1, 0, -1, 1, -1, 1, 0, -1};

void dfs(ll x, ll y) {
    seen[x][y] = true;

    rep(i,0,4) {
        ll nx = x + dx[i];
        ll ny = y + dy[i];

        if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue;
        if(grid[nx][ny] == '#') continue;
        if(seen[nx][ny]) continue;

        parent[nx][ny] = {x,y};
        dfs(nx,ny);
    }
}

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);

    cin >> h >> w;
    grid.resize(h);
    seen.assign(h,vector<bool>(w,false));
    parent.assign(h, vector<pair<int,int>>(w, {-1, -1}));

    int sx, sy, gx, gy; cin >> sx >> sy >> gx >> gy;
    --sx; --sy; --gx; --gy;

    rep(i,0,h) cin >> grid[i];
    // rep(i,0,h) {
    //     rep(j,0,w) {
    //         if(grid[i][j] == 'S') {
    //             sx = i;
    //             sy = j;
    //         }else if(grid[i][j] == 'G') {
    //             gx = i;
    //             gy = j;
    //         }
    //     }
    // }

    dfs(sx,sy);

    //経路復元
    vector<pair<int,int>> path;

    if(seen[gx][gy]) {
        ll x = gx;
        ll y = gy;

        while(!(x == sx && y == sy)) {
            path.push_back({x,y});

            auto[px, py] = parent[x][y];
            x = px;
            y = py;
        }

        path.push_back({sx,sy});

        reverse(nall(path));
    }

    

    if(seen[gx][gy]) cout << "Yes" << endl;
    else cout << "No" << endl;

    return 0;
}
