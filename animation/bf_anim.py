import math
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import sys
import matplotlib.pyplot as plt

plt.ioff()
plt.close("all")

INF = 10**18

# 日本語フォント
mpl.rcParams["font.family"] = "Noto Sans CJK JP"
mpl.rcParams["axes.unicode_minus"] = False

# ====== グラフ例 ======
#いろんなパターン試せます

#わかりやすさ重視？
# N = 5
# edges = [
#     (0, 1, 5),
#     (0, 2, 2),
#     (2, 1, -4),   # 0->2->1 で dist[1] が小さくなる
#     (1, 3, 3),
#     (2, 3, 10),
#     (3, 4, 1),
# ]
# start = 0

# pos = {
#     0: (0, 0),
#     1: (1.2, 1.0),
#     2: (2.0, 0.0),
#     3: (3.0, 1.0),
#     4: (4.2, 0.0),
# }

#100の辺が後で上書きされる
# N = 5
# edges = [
#     (0, 1, 3),
#     (1, 4, 100),  # いったん dist[4] が 103 になりやすい
#     (1, 2, -2),
#     (2, 3, 2),
#     (3, 4, 1),    # 後で dist[4] が 4 に更新される
#     (0, 3, 10),
# ]
# start = 0

# pos = {
#     0: (0, 0),
#     1: (1.2, 1.0),
#     2: (2.0, 0.0),
#     3: (3.0, 1.0),
#     4: (4.2, 0.0),
# }

#
N = 5
edges = [
    (0, 1, 1),
    (1, 2, -2),
    (2, 1, -2),   # 1<->2 が負閉路（回るたびに減る）
    (2, 3, 2),
    (3, 4, 1),
]
start = 0

pos = {
    0: (0, 0),
    1: (1.2, 1.0),
    2: (2.0, 0.0),
    3: (3.0, 1.0),
    4: (4.2, 0.0),
}


# 固定色
node_colors = ["tab:blue", "tab:orange", "tab:green", "tab:red", "tab:purple"]

def fmt(d):
    return "未到達" if d >= INF // 2 else str(d)

def edge_label_xy(a, b, idx, base=0.22):
    """辺ラベル：中点 + 法線/接線方向にずらして被り軽減"""
    x1, y1 = pos[a]
    x2, y2 = pos[b]
    mx, my = (x1 + x2) / 2, (y1 + y2) / 2

    vx, vy = x2 - x1, y2 - y1
    L = math.hypot(vx, vy)
    if L == 0:
        return mx, my

    # 単位ベクトル（接線）
    tx, ty = vx / L, vy / L
    # 単位法線
    nx, ny = -ty, tx

    # 辺ごとに置く側を交互に
    side = 1 if (idx % 2 == 0) else -1

    # ずらし量に少しバリエーション
    off_n = base + 0.06 * (idx % 3)          # 法線方向
    off_t = 0.07 * ((idx % 3) - 1)           # 接線方向（-0.07, 0, +0.07）

    return mx + side * nx * off_n + tx * off_t, my + side * ny * off_n + ty * off_t

# ====== フレーム生成：パス×辺 ======
# dist = [INF] * N
# dist[start] = 0

# frames = []
# frames.append({
#     "pass": 0,
#     "edge_i": -1,
#     "u": None, "v": None, "w": None,
#     "dist": dist.copy(),
#     "relaxed": False,
#     "msg": "初期化：開始点の距離 = 0、それ以外は未到達"
# })

# max_pass = N - 1
# for p in range(1, max_pass + 1):
#     updated_this_pass = False
#     for i, (u, v, w) in enumerate(edges):
#         relaxed = False
#         if dist[u] < INF // 2 and dist[v] > dist[u] + w:
#             dist[v] = dist[u] + w
#             relaxed = True
#             updated_this_pass = True

#         frames.append({
#             "pass": p,
#             "edge_i": i,
#             "u": u, "v": v, "w": w,
#             "dist": dist.copy(),
#             "relaxed": relaxed,
#             "msg": f"パス{p}：辺{i+1}/{len(edges)}を確認 → " + ("更新あり（緩和）" if relaxed else "更新なし")
#         })

#     if not updated_this_pass:
#         frames.append({
#             "pass": p,
#             "edge_i": -2,
#             "u": None, "v": None, "w": None,
#             "dist": dist.copy(),
#             "relaxed": False,
#             "msg": f"パス{p}：更新が起きなかったので終了（これ以上短くならない）"
#         })
#         break
P = N  # N回目も回して負閉路チェック
# P = N - 1  # 通常ならこっち

dist = [INF] * N
dist[start] = 0

frames = []
frames.append({
    "pass": 0,
    "edge_i": -1,
    "u": None, "v": None, "w": None,
    "dist": dist.copy(),
    "relaxed": False,
    "msg": "初期化：開始点の距離 = 0、それ以外は未到達"
})

for p in range(1, P + 1):
    updated_this_pass = False

    for i, (u, v, w) in enumerate(edges):
        relaxed = False
        if dist[u] < INF // 2 and dist[v] > dist[u] + w:
            dist[v] = dist[u] + w
            relaxed = True
            updated_this_pass = True

        frames.append({
            "pass": p,
            "edge_i": i,
            "u": u, "v": v, "w": w,
            "dist": dist.copy(),
            "relaxed": relaxed,
            "msg": f"パス{p}：辺{i+1}/{len(edges)}を確認 → " + ("更新あり（緩和）" if relaxed else "更新なし")
        })

    # ★N回目で更新が起きたら負閉路到達可能
    if p == N and updated_this_pass:
        frames.append({
            "pass": p,
            "edge_i": -2,
            "u": None, "v": None, "w": None,
            "dist": dist.copy(),
            "relaxed": False,
            "msg": "N回目でも更新あり → 負閉路（負のサイクル）が到達可能！"
        })
        break


# ====== 描画：左=グラフ、右=文字 ======
fig, (axG, axT) = plt.subplots(
    1, 2,
    figsize=(14, 6),
    gridspec_kw={"width_ratios": [3.3, 1.2]},
)

def _on_close(event):
    plt.close("all")
    sys.exit(0)

fig.canvas.mpl_connect("close_event", _on_close)


# 上に2段で文字を置く（被らないように領域確保）
fig.subplots_adjust(top=0.84, wspace=0.25)

# 1段目：固定タイトル
fig.suptitle("ベルマンフォード法：辺を1本ずつ確認（緩和の流れ）", fontsize=14, y=0.98)
# 2段目：更新あり/なし等のメッセージ（毎フレーム更新）
msg_text = fig.text(0.02, 0.90, "", fontsize=12, ha="left", va="top")

# 軸範囲固定
xs = [pos[i][0] for i in range(N)]
ys = [pos[i][1] for i in range(N)]
pad = 0.6
xlim = (min(xs) - pad, max(xs) + pad)
ylim = (min(ys) - pad, max(ys) + pad)

def draw(frame):
    axG.clear()
    axT.clear()
    axG.axis("off")
    axT.axis("off")

    axG.set_xlim(*xlim)
    axG.set_ylim(*ylim)

    p = frame["pass"]
    i = frame["edge_i"]
    v = frame["v"]
    dist_now = frame["dist"]
    relaxed = frame["relaxed"]
    msg = frame["msg"]

    # 上のメッセージ更新
    msg_text.set_text(msg)

    # ---- 辺 ----
    for idx, (a, b, c) in enumerate(edges):
        x1, y1 = pos[a]
        x2, y2 = pos[b]

        # デフォルト（薄い灰色）
        color = "gray"
        lw = 2.2
        alpha = 0.55
        mscale = 16

        # 今見てる辺（太い黒）
        if idx == i:
            color = "black"
            lw = 4.8
            alpha = 0.95
            mscale = 22

        # 更新が起きた辺（さらに太く）
        if idx == i and relaxed:
            lw = 6.8
            alpha = 1.0
            mscale = 26

        axG.annotate(
            "",
            xy=(x2, y2),
            xytext=(x1, y1),
            arrowprops=dict(
                arrowstyle="-|>",
                lw=lw,
                color=color,
                alpha=alpha,
                mutation_scale=mscale,
                shrinkA=10,
                shrinkB=14,
            ),
            zorder=1,
        )

        # ★重みラベル（ここがインデント重要：forループの中！）
        lx, ly = edge_label_xy(a, b, idx)
        axG.text(
            lx, ly, f"{c}",
            fontsize=11,
            ha="center", va="center",
            zorder=5,
            bbox=dict(facecolor="white", edgecolor="none", alpha=0.7, pad=1.5)
        )

    # ---- 頂点 ----
    for node in range(N):
        x, y = pos[node]
        s = 950
        edge_lw = 2.0
        apha = 0.95

        if relaxed and node == v:
            s = 1450
            edge_lw = 4.0
            apha = 1.0

        axG.scatter(
            [x], [y],
            s=s,
            c=node_colors[node],
            edgecolors="black",
            linewidths=edge_lw,
            alpha=apha,
            zorder=3,
        )

        label = fmt(dist_now[node])
        fs = 12 if len(label) <= 3 else 10
        axG.text(
            x, y, f"{node}\n{label}",
            ha="center", va="center",
            fontsize=fs,
            color="white",
            zorder=4,
        )

    # ---- 右：距離表＋説明 ----
    axT.text(0.02, 0.95, "距離表 dist（開始点から）", fontsize=12, va="top")

    y = 0.88
    for node in range(N):
        axT.text(0.02, y, f"頂点 {node}: {fmt(dist_now[node])}", fontsize=11, va="top")
        y -= 0.07

    axT.text(
        0.02, 0.35,
        "見方：\n"
        "・太い矢印 = 今確認している辺\n"
        "・さらに太い = 更新（緩和）が発生\n"
        "・大きい点 = 更新された到達先頂点",
        fontsize=11, va="top"
    )

    if i >= 0:
        axT.text(0.02, 0.08, f"現在：パス {p} / 辺 {i+1}/{len(edges)}", fontsize=11)
    else:
        axT.text(0.02, 0.08, f"現在：パス {p}", fontsize=11)

def update(k):
    draw(frames[k])

from matplotlib.widgets import Button

state = {"k": 0}

def redraw():
    draw(frames[state["k"]])
    fig.canvas.draw_idle()

# 最初の1枚を表示
redraw()

# ボタン配置（図の下）
ax_prev = fig.add_axes([0.78, 0.02, 0.08, 0.06])  # [left, bottom, width, height]
ax_next = fig.add_axes([0.88, 0.02, 0.08, 0.06])

btn_prev = Button(ax_prev, "Prev")
btn_next = Button(ax_next, "Next")

def on_prev(event):
    state["k"] = (state["k"] - 1) % len(frames)
    redraw()

def on_next(event):
    state["k"] = (state["k"] + 1) % len(frames)
    redraw()

btn_prev.on_clicked(on_prev)
btn_next.on_clicked(on_next)

# キー操作（←/→でも進める）
def on_key(event):
    if event.key in ("right", "d", "n"):
        on_next(None)
    elif event.key in ("left", "a", "p"):
        on_prev(None)

fig.canvas.mpl_connect("key_press_event", on_key)

plt.show(block=True)
plt.close("all")
