#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define LEN 30
#define TEXTLEN 100

void read_input(char * in) { /* 作成済み */
    scanf("%100s", in);
}
//マークの座標を出力
void print_marks(int n, int * xs, int * ys) { /* 作成済み */
    int i;
    for(i=0; i<n; i++) {
        printf("%2d: %d %d\n", i, xs[i], ys[i]);
    }
}
//配列のスタートからどこまで数字か見てその数を返す。
int count_digits(char * from) { /* 練習問題３ */
    int result=0;
    while(isdigit(from[result])) {
        result++;    
    }
    return result;
}
//fromからtoの区間の文字列を整数にして返す。
int digits_to_int(char * from, char * to) { /* 練習問題４ */
    int i, result = 0;
    for(i=0; from+i < to; i++) {
        int v = from[i]-'0';
        result = result * 10 + v;
    }
    return result;
}
//ここで命令列を処理してマーク生成
int process_insts(char * in, int * xs, int * ys) {
    xs[0] = 0;
    ys[0] = 0;
    int mark_cnt = 1;
    int cur = 0;
    while(in[cur] != NULL) {
        int n = 0,num = 0;
        // printf("aaa");
        //文字だったらその次から関数を動かす
        if(('A' <= in[cur] && in[cur] <= 'Z')) {
            n = count_digits(in+cur+1);
            num = digits_to_int(in+cur+1,in+cur+n);
            printf("%d %d",n,num);
        //東西南北判定
        if(in[cur] == 'N') {
            xs[mark_cnt] = xs[mark_cnt-1];
            ys[mark_cnt] = ys[mark_cnt-1] - num;
            mark_cnt++;
        }else if(in[cur] == 'E') {
            xs[mark_cnt] = xs[mark_cnt-1] + num;
            ys[mark_cnt] = ys[mark_cnt-1];
            mark_cnt++;
        }else if(in[cur] == 'W') {
            xs[mark_cnt] = xs[mark_cnt-1] - num;
            ys[mark_cnt] = ys[mark_cnt-1];
            mark_cnt++;
        } else if(in[cur] == 'S') {
            xs[mark_cnt] = xs[mark_cnt-1];
            ys[mark_cnt] = ys[mark_cnt-1] + num;
            mark_cnt++;
        }
        }
        

        cur += n;
    }
    return 0;
}

void sort_marks(int n, int * xs, int * ys) {

}

int main() {
    int n;
    int Xs[LEN], Ys[LEN];
    char input[TEXTLEN+1];
    read_input(input);
    printf("process_inst-----\n");
    n = process_insts(input, Xs, Ys);
    printf("print marks (placed order)-----\n");
    print_marks(n, Xs, Ys);
    printf("start sorting-----\n");
    sort_marks(n, Xs, Ys);
    printf("print marks (sorted)-----\n");
    print_marks(n, Xs, Ys);
    return 0;
}
