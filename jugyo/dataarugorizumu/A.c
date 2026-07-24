
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define RECORD_MAX (10000)

struct Record {
    int key;    /* 重複しない */
    int value;  /* 0以上と仮定 */
};

struct Record table[RECORD_MAX]; /* 表 */
int n = 0;                       /* レコード数(<= RECORD_MAX) */

void regist(int key, int value) {
    int i, j;
    for (i = 0; i < n; i++) {
        if (table[i].key == key) {
            /* keyと一致するレコードがすでにあった場合 */
            table[i].value = value;
            return;
        } else if (table[i].key > key) {
            if (n < RECORD_MAX) {
                /* i番目以降のレコードを1つづつ後ろにずらす */
                for (j = n - 1; j >= i; j--) {
                    table[j+1].key = table[j].key;
                    table[j+1].value = table[j].value;
                }
                /* i番目のレコードに書き込み */
                table[i].key = key;
                table[i].value = value;                
                n++;
                return;
            }
        }
    }
    /* keyの値が一番大きい場合、一番最後に書き込み */
    if (n < RECORD_MAX) {
        table[n].key = key;
        table[n].value = value;
        n++;
    }
}

int search(int key) {
    int i, low = 0, high = n, middle = ((high)/2);
    while(1) {
        if(table[middle-1].key < key) {
            low = middle;
            middle = low + ((high-low)/2);
        }else if(table[middle-1].key > key) {
            high = middle;
            middle = ((high)/2);
        }
        if(table[middle-1].key == key) return table[middle-1].value;
    }
    return -1;
}

int main() {
    int n, m, i, key, value;

    scanf("%d", &n);
    printf("レコード数(%d)\n", n);
    for (i = 0; i < n; i++) {
        scanf("%d", &key);
        scanf("%d", &value);
        printf("キー(%d), 値(%d)を登録\n", key, value);
        regist(key, value);
    }
    scanf("%d", &m);
    for (i = 0; i < m; i++) {
        scanf("%d", &key);
        printf("キー(%d)を探索．\n", key);
        value = search(key);
        if (value == -1) {
            printf("キー(%d)を持つレコードは登録されていません．\n", key);
        } else {
            printf("キー(%d)の値は%dです．\n", key, value);
        }
    }
}