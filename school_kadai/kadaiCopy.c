/*
2025/12/16 12571018  大谷優豊
 */
#include<stdio.h>
#define LEN 16
#define HIGH 8
int *V;
int Round = 0;

int A[HIGH][LEN];
//盤面の読み込み
void read_input(void) {
    int x,y;
    for(y=HIGH-1; y>=0; y--) {
        for(x=0; x<LEN; x++) {
            int v;
            scanf("%1d", &v);
            A[y][x] = v;
        }
    }
}
//盤面の表示
void print_board(void) {
    int x,y;
    for(y=HIGH-1; y>=0; y--) {
        for(x=0; x<LEN; x++) {
            printf("%1d", A[y][x]);
        }
        printf("\n");
    }
}
//連続する数をカウント
int scan_row(int x, int y, int *v) {
    int len = 0;
    int target = A[y][x];
    if(target == 0) *v = 0;
    else {
        *v = target;
        while(x + len < LEN && A[y][x + len] == target) {
            len++;
        }
    }
    return len;
}
//連続する場所を0で埋める
void make_zero(int x, int y, int len) {
    int i;
    for(i=0; i<len; i++) {
        A[y][x + i] = 0;
    }
}
//指定行の「消し」処理を管理
int process_row(int y) {
    int score = 0;
    for(int x = 0; x < LEN;x++) {
        int v,len;
        len = scan_row(x,y,&v);
        if(len >= 3) {
            make_zero(x,y,len);
            score += v*len*len;
        }
    }
    return score;
}
//すべての行の「消し」処理を管理
int process_rows(void) {
    int sum = 0;
    for(int y = 0; y < HIGH; y++) {
        int s = process_row(y);
        sum += s;
    }
    printf("Round %d \n",Round);
    Round++;
    return sum;
}
//列を下に詰める
void slide_column(int x) {
    /* Step 5 */
    int tmp = 0;
    int cur = 0;
        for(int i = 0; i < HIGH; i++) {
            if(A[i][x] != 0) {
                A[tmp][x] = A[i][x];
                tmp++;
            }
        }
        for(int i = tmp; i < HIGH;i++) {
            A[i][x] = 0;
        }
}
//すべての列を下に詰める
void slide_columns(void) {
    /* Step 5 */
    for(int x = 0; x < LEN;x++) slide_column(x);
}

void test1(void) {
    int x = 3, y= 0, v = 0;
    int len = scan_row(x, y, &v); 
    printf("test1----------\n");
    printf("scan_row from (%d, %d) is len:%d, v:%d\n", 
        x, y, len, v);
        print_board();
}
void test2(void) {
    /* STEP2 確認用：変えないで */    
    printf("test2----------\n");
    make_zero(3, 6, 3); 
    print_board();
}
void test3(void) {
    /* STEP3 確認用：変えないで */
    /* 6行目で試す*/ 
    int result = process_row(0); 
    printf("test3: result %d -------\n", result);
    print_board();
}
void test4and5(void) {
    /* STEP4 and 5 確認用：変えないで */
    int score;
    printf("test4: -------\n");
    score =process_rows();
    printf("score: %d\n", score);
    print_board();
    printf("test5: -------\n");
    slide_columns();
    print_board();
}
//メインの処理ループ
void main_loop(void) {
    int SUM = 0,SCORE = 0;
    while(1) {
        SCORE = process_rows();
        SUM += SCORE;
        print_board();
        slide_columns();
        printf("score: %d (+%d)\n",SUM,SCORE);

        if(SCORE == 0) break;
    }
    printf("Finished!\n");
}
//感想
//作るの大変でした。難しいというより複雑でした。
// ポインタのことと、関数のことを学べる良い問題だと思いました。

#define TEST 6 /* step にあわせて数を変えていこう */
int main(void) { /* 変更する必要なし*/
    read_input();
    print_board();
    switch(TEST) {
        case 1:
            test1();
            break;
        case 2:
            test2();
            break;
        case 3:
            test3();        
            break;
        case 4:
        case 5:        
            test4and5();        
            break;
        case 6:
            main_loop();
            break;
        default:
            printf("TEST is %d. Please set TEST to 1--6.\n", TEST);
    }
    return 0;
}

/*
0000000000000000
9912222533373230
9925274448194750
4639997711232190
3143533353322480
4421358885221340
1213322113533130
2134441335999310
*/

/*  出力結果 
0000000000000000
9912222533373230
9925274448194750
4639997711232190
3143533353322480
4421358885221340
1213322113533130
2134441335999310
0000000000000000
9912222533373230
9925274448194750
4639997711232190
3143533353322480
4421358885221340
1213322113533130
2134441335999310
Round 0 
0000000000000000
9910000500073230
9925270008194750
4630007711232190
3143500053322480
4421350005221340
1213322113533130
2130001335000310
score: 392 (+392)
Round 1 
0000000000000000
9910000000000230
9920000008073750
4630000001194190
3145200513232480
4423500055322340
1211352113221130
2100021335500010
score: 509 (+117)
Round 2 
0000000000000000
9900000000000030
9910000008000250
4620000001173790
3130000013294180
0005200000332440
1223552113000330
2000321335520000
score: 626 (+117)
Round 3 
0000000000000000
0000000000000000
9000000000000030
9900000008000250
4910000001170790
3620200013293180
1130002110000000
0003321335522330
score: 752 (+126)
Round 4 
0000000000000000
0000000000000000
0000000000000000
9000000000000030
9900000008000250
4910000000070790
3620202113293180
1100021335522330
score: 788 (+36)
Round 5 
0000000000000000
0000000000000000
0000000000000000
9000000000000030
9900000000000250
4900000008070790
3610002113293180
1120221335522330
score: 788 (+0)
Finished!　*/
