// kadai14.c
#include <stdio.h>
#define MAX_CUSTOMERS (100)

int  calc_single_fare(int ride_time, int ride_dist);
int  calc_all_fares( int ride_time[],  int ride_dist[],int ride_fare[], int n);
void print_results( int ride_time[],  int ride_dist[],int ride_fare[], int n, int total_fare);

int main(void) {
    int ride_time[MAX_CUSTOMERS] = {0};
    int ride_dist[MAX_CUSTOMERS] = {0};
    int ride_fare[MAX_CUSTOMERS] = {0};
    int total_fare = 0;
    int n = 0;

    //入力
    do {
        printf("%d番目の客：乗車時刻と距離? ", n + 1);
        if (scanf("%d%d", &ride_time[n], &ride_dist[n]) != 2) return 0;
        n++;
    } while (0 <= ride_time[n - 1] && 0 <= ride_dist[n - 1]);
    n--;

    //運賃の計算
    total_fare = calc_all_fares(ride_time, ride_dist, ride_fare, n);

    //結果を表示する
    print_results(ride_time, ride_dist, ride_fare, n, total_fare);
    return 0;
}

//運賃の計算をする。
int calc_all_fares(int ride_time[],  int ride_dist[],int ride_fare[], int n) {
    int total = 0;
    for (int i = 0; i < n;i++) {
        ride_fare[i] = calc_single_fare(ride_time[i], ride_dist[i]);
        total += ride_fare[i];
    }
    return total;
}

//結果を表示する関数
void print_results( int ride_time[],  int ride_dist[], int ride_fare[], int n, int total_fare) {
    for (int i = 0; i < n; i++) {
        printf("%d番目の客：乗車時刻%02d時 / 乗車距離%5dm ＝ 料金%d5円\n",
               i + 1, ride_time[i], ride_dist[i], ride_fare[i]);
    }
    printf("合計売上げ：%d\n", total_fare);
}

//乗車時刻と距離から運賃を計算する関数
int calc_single_fare(int ride_time, int ride_dist)
{
     int first_dist = 1000;
     int first_fare = 650;
     int add_dist = 250;
     int add_fare = 100;

    int fare = first_fare;

    if (ride_dist > first_dist) {
        int extra = ride_dist - first_dist;
        // 切り上げ処理
        int steps = (extra + add_dist - 1) / add_dist;
        fare += steps * add_fare;
    }

    //深夜料金2割増しの場合
    if (ride_time >= 22 || ride_time < 5) {
        fare = (fare * 12) / 10;
    }

    return fare;
}
