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

        q = ask(i + 1, argv[3]);

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
    output_file = fopen(argv[2], "r");
    for(int i = 0; i < N; i++) {
        free(S[i]);
    }
    free(S);

    return 0;
}