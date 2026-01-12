#include<stdio.h>
#define LEN 16
#define HIGH 8

int A[HIGH][LEN];
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
void print_board(void) {
    int x,y;
    for(y=HIGH-1; y>=0; y--) {
        for(x=0; x<LEN; x++) {
            printf("%1d", A[y][x]);
        }
        printf("\n");
    }
}

int scan_row(int x, int y, int * v) {
    /* Step 1 */
    return 0; /* コンパイル通す用*/
}
void make_zero(int x, int y, int len) {
    /* Step 2 */
}
int process_row(int y) {
    /* Step 3 */
    return 0; /* コンパイル通す用*/
}
int process_rows(void) {
    /* Step 4 */
    return 0; /* コンパイル通す用*/
}

void slide_column(int x) {
    /* Step 5 */
}
void slide_columns(void) {
    /* Step 5 */
}

void test1(void) {
    /* STEP1 確認用：変えないで */
    int x = 2, y= 0, v = 0;
    int len = scan_row(x, y, &v); 
    printf("test1----------\n");
    printf("scan_row from (%d, %d) is len:%d, v:%d\n", 
        x, y, len, v);
    x += len;
    len = scan_row(x, y, &v); 
    printf("scan_row from (%d, %d) is len:%d, v:%d\n", 
        x, y, len, v);
    y=HIGH-1;
    len = scan_row(x, y, &v); 
    printf("scan_row from (%d, %d) is len:%d, v:%d\n", 
        x, y, len, v);
}
void test2(void) {
    /* STEP2 確認用：変えないで */    
    printf("test2----------\n");
    make_zero(3, 0, 3); 
    print_board();
}
void test3(void) {
    /* STEP3 確認用：変えないで */
    /* 6行目で試す*/ 
    int result = process_row(6); 
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

void main_loop(void) {
    /* Step 6 */
}

#define TEST 1 /* step にあわせて数を変えていこう */
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
