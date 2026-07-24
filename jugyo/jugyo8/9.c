#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct log{
    char date[10+1];
    char time[8+1];
    char category[5+1];
} log;

int main(void) {
    
    FILE *fp;
    fp = fopen("log.txt", "r");
    if (fp == NULL) {
        printf("ファイルをオープンできません\n");
        exit(1);
    }

    int i = 0;
    int cnt = 0;
    char tmp_date[20], tmp_time[20], tmp_category[20];
    while(fscanf(fp, "%s %s %s", tmp_date, tmp_time,tmp_category) != EOF) {
        i++;
        cnt++;
    };
    fclose(fp);
    fp = fopen("log.txt", "r");
    if (fp == NULL) {
        printf("ファイルをオープンできません\n");
        exit(1);
    }
    
    log *DATA;
    //メモリ確保
    DATA = (log *)malloc(sizeof(log) * cnt);
    if(!DATA) {
        printf("メモリが確保できませんでした。\n");
        exit(1);
    }
    i = 0;
    while(fscanf(fp, "%s %s %s", DATA[i].date, DATA[i].time, DATA[i].category) != EOF) {
        i++;
    };

    FILE *fpp;
    fpp = fopen("error_log.txt", "w");
    if (fpp == NULL) {
        printf("ファイルをオープンできません\n");
        exit(1);
    }

    for(i = 0; i < cnt; i++) {
        if(DATA[i].category == "ERROR") {
            fprintf(fpp, "%s", DATA[i].date);
            fprintf(fpp, "%s", DATA[i].time);
            fprintf(fpp, "%s", DATA[i].category);
            fprintf(fpp, "\n");
        }
    }
    
    printf("\n");
    
    free(DATA);
    fclose(fp);
    return 0;
}