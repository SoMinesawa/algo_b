#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "ask.h"
#define max(a, b) ((a) > (b) ? (a) : (b))

int count_correct = 0;
int count_ask = 0;

int min(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

int calc_edit_distance(char *str1, char *str2) {
    int lenstr1 = strlen(str1) + 1;
    int lenstr2 = strlen(str2) + 1;
    int d[lenstr1][lenstr2];
    int i1 = 0, i2 = 0, cost = 0;

    for (; i1 < lenstr1; i1++) d[i1][0] = i1;
    for (; i2 < lenstr2; i2++) d[0][i2] = i2;

    for (i1 = 1; i1 < lenstr1; i1++) {
        for (i2 = 1; i2 < lenstr2; i2++) {
            cost = str1[i1 - 1] == str2[i2 - 1] ? 0 : 1;
            d[i1][i2] = min(d[i1 - 1][i2] + 1, d[i1][i2 - 1] + 1,
                            d[i1 - 1][i2 - 1] + cost);
        }
    }
    return d[lenstr1 - 1][lenstr2 - 1];
}

int main(int argc, char* argv[]) {

    srand((unsigned int)time(NULL));
    FILE* input_file = fopen(argv[1], "r");
    FILE* output_file = fopen(argv[2], "w");
    FILE* answer_file = fopen(argv[3], "r");

    if(input_file == NULL || output_file == NULL || answer_file == NULL) {
        fprintf(stderr, "error\n");
        exit(EXIT_FAILURE);
    }

    int p_ins, p_sub, p_del;
    fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);

    char** S = (char**)malloc(sizeof(char*) * N);

    for(int i = 0; i < N; i++) {
        S[i] = (char*)malloc(sizeof(char) * (DATA_LENGTH + 1));
        fscanf(input_file, "%s", S[i]);
    }



    for(int i = 0; i < Q; i++) {
        char* q = (char*)malloc(sizeof(char) * 200);
        fscanf(input_file, "%s", q);

        /*
        q = ask(i + 1, argv[3]);
        count_ask++; //askを使ったときはcount_askにプラス
        */

        int min_edit_distance = INT_MAX;
        int min_edit_distance_index = 0;
        for(int j = 0; j < N; j++) {
            char tmp[101];
            for(int i = 0; i < 50; i++){
              strncpy(tmp, &S[j][100*i], 101);
              int edit_distance = calc_edit_distance(q, tmp);
              if(edit_distance < min_edit_distance) {
                min_edit_distance = edit_distance;
                min_edit_distance_index = j;
              }
            }
            
            
        }
        fprintf(output_file, "%d\n", min_edit_distance_index + 1);
        
    }
    fclose(input_file);
    fclose(output_file);

    for(int i = 0; i < N; i++) {
        free(S[i]);
    }
    free(S);

    //CPU時間(採点前)
    clock_t cpu_time = clock();
    double sec = (double)cpu_time / CLOCKS_PER_SEC;
    printf("CPU時間: %fs\n", sec);

    //ピークメモリ
    system("/usr/bin/time -v ./run_G1 2> info.txt");
    FILE *info_file;
    info_file = fopen("info.txt", "r");
    char c;
    char tmp[100];
    for(int i=0; i<12; i++){
        fgets(tmp, 100, info_file);
    }
    printf("ピークメモリ: ");
    for(int i=37; tmp[i+1]!=NULL; i++){
        printf("%c",tmp[i]);
    }
    printf("kbytes\n");
    fclose(info_file);

    //スコア採点
    output_file = fopen(argv[2], "r");
    int out;
    int answer;
    fscanf(answer_file, "%d %d %d", &p_ins, &p_sub, &p_del);
    for(int i = 0; i < N; i++) {
        fscanf(output_file, "%d", &out);
        fscanf(answer_file, "%d %s", &answer, tmp);
        if(out == answer){
            count_correct++;
        }
    }
    fclose(output_file);
    fclose(answer_file);
    printf("スコア: %d/10000\n", count_correct*100 - count_ask*5);
    printf("├正答数: %d/100\n", count_correct);
    printf("└ask回数: %d\n", count_ask);

    return 0;
}