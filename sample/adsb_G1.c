#include <stdio.h>
#include <stdlib.h>

#include "ask.h"
#define max(a, b) ((a) > (b) ? (a) : (b))

int calc_similarity(char* s1, char* s2) {
    int m = strlen(s1);
    int n = strlen(s2);

    // 2次元配列を動的に確保する
    int** dp = (int**)malloc(sizeof(int*) * (m + 1));
    for(int i = 0; i <= m; i++) {
        dp[i] = (int*)malloc(sizeof(int) * (n + 1));
    }

    // LCSを求める
    for(int i = 0; i <= m; i++) {
        for(int j = 0; j <= n; j++) {
            if(i == 0 || j == 0) {
                dp[i][j] = 0;
            } else if(s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }

    int lcs = dp[m][n];

    // 2次元配列を解放する
    for(int i = 0; i <= m; i++) {
        free(dp[i]);
    }
    free(dp);

    return lcs;
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

        int max_similarity = 0;
        int max_similarity_index = 0;
        for(int j = 0; j < N; j++) {
            int similarity = calc_similarity(q, S[j]);
            if(similarity > max_similarity) {
                max_similarity = similarity;
                max_similarity_index = j;
            }
        }

    fclose(input_file);
    fclose(output_file);
    for(int i = 0; i < N; i++) {
        free(S[i]);
    }
    free(S);

    return 0;
    }
}


