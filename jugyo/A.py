opt_val = 0             # 最適値（最大値）を0で初期化
opt_x = []              # 最適解
inf = 1<<60;
mx = -inf;
for x in product([0, 1], repeat=18):
    sumB = 0;
    sumC = 0;
    for i in range(18):
        if(x[i] == 1):
            sumB += b[i];
            sumC += c[i];
    if(sumC <= C  && sumB > mx):
        mx = sumB;
print(f'最適値 = {opt_val}')
print(f'最適解 = {opt_x}')