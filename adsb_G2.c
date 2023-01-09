#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ask.h"

#define MAX_l 11
#define MIN_l 7
#define min(A, B, C) (A > B ? (B > C ? C : B) : (A > C ? C : A))

typedef struct {
    int channel;
    int start;
} Item;

Item NULLitem = {-1, -1};

typedef struct STnode* link;
struct STnode {
    Item item;
    link next;
};

int power(int x, int y) {
    int a = 1;
    for(int i = 0; i < y; i++) {
        a = a * x;
    }
    return a;
}

link NEW(Item item, link next) {
    link x = malloc(sizeof * x);
    x->item = item;
    x->next = next;
    return x;
}

link** heads;
link z;
double p_nerr;

void STinit() {
    heads = (link**)malloc(sizeof(link*) * MAX_l);
    z = NEW(NULLitem, NULL);
    for(int l = MIN_l; l <= MAX_l; l++) {
        int M = power(4, l); // 配列のサイズ
        heads[l] = (link*)malloc(M * sizeof(link));
        for (int i = 0; i < M; i++){
            heads[l][i] = z;
        }
    }
}

// char*型の4進数をint型の10進数へ
int hash(int l, char* str) {
    int a = 0;
    for (int i = 0; i < l; i++) {
        a += (str[i] - '0') * power(4, l-i-1);
    }
    return a;
}

void STinsert(int l, char* str, int channel, int start) {
    int i = hash(l, str);
    Item item = {channel, start};
    heads[l][i] = NEW(item, heads[l][i]);
}

link STsearch(int l, char* v) {
    return heads[l][hash(l, v)];
}

int calc_edit_dis(char* str1, char* str2) {
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

char* scanf_from_file(FILE* file, int len) {
    char* str = (char*)malloc(sizeof(char) * (len));
    fscanf(file, "%s", str);
    return str;
}

char* slice(char* str, int offset, int len) {
    char* sub_str = (char*)malloc(len + 1);
    strncpy(sub_str, str + offset, len);
    sub_str[len] = '\0';
    return sub_str;
}

void insert(int l, char* S, int ch) {
    for(int offset = 0; offset < DATA_LENGTH - l; offset++) {
        STinsert(l, slice(S, offset, l), ch, offset);
    }
}

int search(char** S, char* q) {
    link p;
    int edit_dis;
    int min_dis = 100;
    int min_dis_channel = randint(0, N);
    int L = strlen(q);
    int t = 0.225 * L; // 編集距離の閾値
    
    for(int l = MAX_l; l >= MIN_l; l--) {
        for (int i = 0; i <= L - l; i++) {
            char* sub_q = slice(q, i, l); //クエリからl文字取り出す
            p = STsearch(l, sub_q); //クエリから取り出したl文字を探索
            while(1) {
                if (p == z) {
                    break; // 一致する部分列はもうない
                }
                int offset = p->item.start - i;
                if (offset + L <= DATA_LENGTH && offset >= 0) {
                    char* sub_s = slice(S[p->item.channel], offset, L);
                    // 位置を合わせた基地局データとクエリとの編集距離を算出
                    edit_dis = calc_edit_dis(sub_s, q);
                    free(sub_s);
                    if (edit_dis <= t) {
                        return p->item.channel + 1; // 編集距離が閾値以下であればそのときの基地局番号を答えとして返す
                    } else if (min_dis > edit_dis){
                        min_dis = edit_dis; // 編集距離の最小値
                        min_dis_channel = p->item.channel; //編集距離が最小値の基地局番号
                    }
                }
                p = p->next;
            }
            free(sub_q);
        }
    }
    // 編集距離が最小値の基地局番号を答えとして返す
    printf("Return minmal distance channel\n");
    return min_dis_channel + 1;
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));
    FILE* input_file = fopen(argv[1], "r");
    FILE* output_file = fopen(argv[2], "w");
    FILE* answer_file = fopen(argv[3], "r");

    if (input_file == NULL || output_file == NULL || answer_file == NULL) {
        fprintf(stderr, "error\n");
        exit(EXIT_FAILURE);
    }

    int p_ins, p_sub, p_del;
    fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);

    p_nerr = (100.0 - p_ins) * (100.0 - p_sub) * (100.0 - p_del) / 1000000;

    printf("Start costruct hashtable with chaining\n");
    STinit();
    
    // 基地局のデータをlずつに分割して、それぞれ基地局番号と開始位置をハッシュ表に格納する
    // 基地局番号はここでは0〜99として後から+1する
    char** S = (char**)malloc(sizeof(char*) * N);
    for (int i = 0; i < N; i++) {
        S[i] = scanf_from_file(input_file, DATA_LENGTH + 1);
        for(int l = MIN_l; l <= MAX_l; l++) {
            insert(l, S[i], i);
        }
    }
    printf("Finish costruct hashtable\n");

    // ↓↓↓　探索開始
    for (int i = 0; i < Q; i++) {
        char* q = scanf_from_file(input_file, 200);
        // q = ask(i + 1, argv[3]); memo : how to call ask method
        printf("%d query:%s\n", i + 1, q);

        int ans = search(S, q);
        fprintf(output_file, "%d\n", ans);
        free(q);
    }

    // ↑↑↑　探索終わり

    fclose(input_file);
    fclose(output_file);
    for (int i = 0; i < N; i++) {
        free(S[i]);
    }
    free(S);
    for (int l = MIN_l; l <= MAX_l; l++) {
        free(heads[l]);
    }
    free(heads);

    /*
    // 以下結果の評価
    // CPU時間(採点前)
    clock_t cpu_time = clock();
    double sec = (double)cpu_time / CLOCKS_PER_SEC;
    printf("CPU時間: %fs\n", sec);

    // スコア採点
    char tmp[100];
    output_file = fopen(argv[2], "r");
    int count_correct = 0;
    int out;
    int answer;
    fscanf(answer_file, "%d %d %d", &p_ins, &p_sub, &p_del);
    for (int i = 0; i < N; i++) {
        fscanf(output_file, "%d", &out);
        fscanf(answer_file, "%d %s", &answer, tmp);
        if (out == answer) {
            count_correct++;
        }
    }
    fclose(output_file);
    fclose(answer_file);
    printf("スコア: %d/10000\n", count_correct * 100 - count_ask * 5);
    printf("├正答数: %d/100\n", count_correct);
    printf("└ask回数: %d\n", count_ask);
    */
    return 0;
}