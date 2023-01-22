#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ask.h"

#define MAX_l 10
#define MIN_l 7

int levenshtein_bitpal64(char* A, int m, char* B, int n) {
    uint64_t PMs[256] = {0};
    for(int i = 0; i < m; i++) {
        unsigned char c = A[i];
        PMs[c] |= 1ull << i;
    }
    int D = m;
    uint64_t D0, HP, HN, VP, VN;
    uint64_t top = 1ull << (m - 1);
    VP = ~0;
    VN = 0;
    for(int i = 0; i < n; i++) {
        unsigned char c = B[i];
        uint64_t PM = PMs[c];
        D0 = (((PM & VP) + VP) ^ VP) | PM | VN;
        HP = VN | ~(D0 | VP);
        HN = D0 & VP;
        if ((HP & top) != 0) {
            ++D;
        } else if ((HN & top) != 0) {
            --D;
        }
        VP = (HN << 1) | ~(D0 | ((HP << 1) | 1));
        VN = D0 & ((HP << 1) | 1);
    }
    return D;
}

int levenshtein_bitpal128(char* A, int m, char* B, int n) {
    if(m > 128) {return -1;}
    uint64_t PMl[256] = {0};
    uint64_t PMh[256] = {0};
    for (int i = 0; i < m; i++) {
        unsigned char c = A[i];
        if(i < 64) {
            PMl[c] |= 1ull << i;				
        } else {
            PMh[c] |= 1ull << (i - 64);								
        }
    }
    int D = m;
    uint64_t D0, HP, HN, VP, VN;
    uint64_t D0_h, HP_h, HN_h, VP_h, VN_h;
    uint64_t top = 1ull << (m - 1);
    if(m > 64) {
        top = 1ull << (m - 65);
    }
    VP = ~0; VN = 0;
    VP_h = ~0; VN_h = 0;
    for (int i = 0; i < n; i++) {
        unsigned char c2 = B[i];
        uint64_t PM = PMl[c2];
        uint64_t PM_h = PMh[c2];
        uint64_t x = (PM & VP); 
        //check x + VP is overflow
        //(cf. Hacker's Delight 2-12)
        uint64_t carry = ((x & VP) | ((x | VP) & ~(x + VP))) >> 63;
        D0 = ((x + VP) ^ VP) | PM | VN;
        D0_h = (((PM_h & VP_h) + VP_h + carry) ^ VP_h) | PM_h | VN_h;
        HP = VN | ~(D0 | VP);  HP_h = VN_h | ~(D0_h | VP_h);
        HN = D0 & VP; HN_h = D0_h & VP_h;
        if(m <= 64) {
            if ((HP & top) != 0) {++D;}
            else if ((HN & top) != 0) {--D;}				
        }else {
            if ((HP_h & top) != 0) {++D;}
            else if ((HN_h & top) != 0) {--D;}	
        }
        uint64_t H_h = (HP_h << 1) | (HP >> 63);
        uint64_t H2_h = (HN_h << 1) | (HN >> 63);
        VP = (HN << 1) | ~(D0 | ((HP << 1) | 1ull));
        VP_h = H2_h | ~(D0_h | H_h);
        VN = D0 & ((HP << 1) | 1ull);
        VN_h = D0_h & H_h;
    }
    return D;
}

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

int search(char** S, char* q, int L, double a, int l) {
    link p;
    int edit_dis;
    int min_dis = 100;
    int min_dis_channel = 0;
    int t = a * L;
    
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
                if (L < 65) {
                    edit_dis = levenshtein_bitpal64(q, L, sub_s, L);
                } else {
                    edit_dis = levenshtein_bitpal128(q, L, sub_s, L);
                }

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

    double p_nerr = (100.0 - p_ins) * (100.0 - p_sub) * (100.0 - p_del) / 1000000;

    int l = 10;
    double a = 0.25;
    if (p_nerr < 0.7786) {
        l = 8;
        a = 0.24;
    } else if (p_nerr < 0.819) {
        l = 8;
        a = 0.23;
    } else if (p_nerr < 0.8299) {
        l = 8;
        a = 0.22;
    } else if (p_nerr < 0.8565) {
        l = 9;
        a = 0.21;
    } else if (p_nerr < 0.88445) {
        l = 9;
        a = 0.2;
    } else if (p_nerr < 0.8938) {
        l = 10;
        a = 0.19;
    } else if (p_nerr < 0.9119) {
        l = 10;
        a = 0.18;
    } else {
        l = 10;
        a = 0.17;
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

        int L = strlen(q);
        int previous_answers[10] = {-1};
        int ans = search(S, q, L, a, l);
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