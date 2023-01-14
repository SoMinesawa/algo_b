#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ask.h"

#define MAX_l 11
#define MIN_l 7

// ↓ 編集距離 onp https://github.com/cubicdaiya/onp/tree/master/c
#ifndef ONP_H
#define ONP_H

typedef unsigned int uint;
typedef char onp_sequence_t;
#define onp_swap(type, val, a, b) type val=a;a=b;b=val;
#define onp_max(a, b) ((a) > (b) ? (a) : (b))
#define onp_seq_len(seq) strlen(seq)

typedef struct {
    onp_sequence_t *a;
    onp_sequence_t *b;
    uint m;
    uint n;
    int *path;
    uint editdis;
} onp_diff_t;

onp_diff_t *onp_alloc_diff (onp_sequence_t *a, onp_sequence_t *b);
void onp_free_diff (onp_diff_t *diff);
void onp_compose (onp_diff_t *diff);

#endif // ONP_H

static uint snake (onp_diff_t *diff, int k, int p, int pp);
static uint snake (onp_diff_t *diff, int k, int p, int pp) {
    int y = onp_max(p, pp);
    int x = y - k;
    while (x < diff->m && y < diff->n && diff->a[x] == diff->b[y]) {
        ++x;
        ++y;
    }
    return y;
}

onp_diff_t *onp_alloc_diff (onp_sequence_t *a, onp_sequence_t *b) {
    onp_diff_t *diff = (onp_diff_t *)malloc(sizeof(onp_diff_t));
    diff->a = a;
    diff->b = b;
    diff->m = onp_seq_len(a);
    diff->n = onp_seq_len(b);
    if (diff->m > diff->n) {
        onp_swap(onp_sequence_t*, seq_tmp, a, b);
        onp_swap(int, int_tmp, diff->m, diff->n);
    }
    diff->editdis = 0;
    return diff;
}

void onp_free_diff (onp_diff_t *diff) {
    free(diff->path);
    free(diff);
    diff = NULL;
}

void onp_compose (onp_diff_t *diff) {
    int  p      = -1;
    int  delta  = diff->n - diff->m;
    uint offset = diff->m + 1;
    uint size   = diff->m + diff->n + 3;
    int *fp     = (int *)malloc(sizeof(int) * size);
    diff->path  = (int *)malloc(sizeof(int) * size);
    for (int i=0;i<size;++i) {
        fp[i] = diff->path[i] = -1;
    }
    do {
        ++p;
        for (int k=-p;k<=delta-1;++k) {
            fp[k+offset] = snake(diff, k, fp[k-1+offset]+1, fp[k+1+offset]);
        }
        for (int k=delta+p;k>=delta+1;--k) {
            fp[k+offset] = snake(diff, k, fp[k-1+offset]+1, fp[k+1+offset]);
        }
        fp[delta+offset] = snake(diff, delta, fp[delta-1+offset]+1,
                                 fp[delta+1+offset]);
    } while (fp[delta+offset] != diff->n);
    diff->editdis = delta + 2 * p;
    free(fp);
}
// ↑ 編集距離 onp

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
double a; // 閾値を決めるときの定数

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

int search(char** S, char* q, int query_number, char* answer_filename, int ask_count) {
    link p;
    int edit_dis;
    int min_dis = 100;
    int min_dis_channel = randint(0, N);
    int L = strlen(q);
    int t = a * L; // 編集距離の閾値
    
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
                    onp_diff_t *diff = onp_alloc_diff(sub_s, q);
                    onp_compose(diff);
                    edit_dis = diff->editdis;
                    onp_free_diff(diff);

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
    if (ask_count < 2) {
        ask_count++;
        q = ask(query_number + 1, answer_filename);
        return search(S, q, query_number, answer_filename, ask_count);
    } else {
        // 編集距離が最小値の基地局番号を答えとして返す
        printf("Return minmal distance channel\n");
        return min_dis_channel + 1;
    }
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

    if (p_nerr < 0.76) {
        a = 0.3;
    } else if (p_nerr < 0.85) {
        a = 0.28;
    } else if (p_nerr < 0.93) {
        a = 0.27;
    } else {
        a = 0.25;
    }

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

        int ans = search(S, q, i, argv[3], 0);
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