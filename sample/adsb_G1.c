#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "ask.h"
#define max(a, b) ((a) > (b) ? (a) : (b))

int min(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

int calc_edit_distance(const char* str1, const char* str2) {
    // str1の文字数をn、str2の文字数をmとする
    int n = strlen(str1);
    int m = strlen(str2);

    // 配列dを宣言する
    // d[i][j]は、str1のi文字目から末尾までと、str2のj文字目から末尾までの
    // 編集距離を表す
    int d[n + 1][m + 1];

    // 配列dを初期化する
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            if (i == 0) {
                // str1が空文字列の場合、str2をすべて挿入する必要がある
                d[i][j] = j;
            } else if (j == 0) {
                // str2が空文字列の場合、str1をすべて削除する必要がある
                d[i][j] = i;
            } else {
                // str1とstr2の末尾の文字が等しい場合、
                // str1のi-1文字目から末尾までと、str2のj-1文字目から末尾までの
                // 編集距離に1を足さずに、そのままd[i][j]とする
                // 等しくない場合、str1の末尾の文字をstr2の末尾の文字で置き換える必要がある
                d[i][j] =
                    min(d[i - 1][j] + 1, d[i][j - 1] + 1,
                        d[i - 1][j - 1] + str1[i - 1] == str2[j - 1] ? 0 : 1);
            }
        }
    }
    // 編集距離を返す
    return d[n][m];
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

        q = ask(i + 1, argv[3]);

        int min_edit_distance = INT_MAX;
        int min_edit_distance_index = 0;
        for(int j = 0; j < N; j++) {
            int edit_distance = calc_edit_distance(q, S[j]);
            if(edit_distance < min_edit_distance) {
                min_edit_distance = edit_distance;
                min_edit_distance_index = j;
            }
        }
        fprintf(output_file, "%d\n", min_edit_distance_index + 1);
    }
    fclose(input_file);
    fclose(output_file);
    output_file = fopen(argv[2], "r");
    for(int i = 0; i < N; i++) {
        free(S[i]);
    }
    free(S);

    return 0;
}
