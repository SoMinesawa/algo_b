// コンパイル: gcc adsb_G2.c -o run_G2 -lm -O2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ask.h"

typedef int Key;
typedef struct {
    Key key;
    int channel;
    int start;
} Item;
#define key(A) (A.key)

#define less(A, B) (A < B)
#define exch(A, B)  \
    {               \
        Item t = A; \
        A = B;      \
        B = t;      \
    }
#define compexch(A, B) \
    if (less(B, A)) exch(A, B)
#define eq(A, B) (A == B)

#define hl h->l
#define hr h->r
#define hll h->l->l
#define hlr h->l->r
#define hrl h->r->l
#define hrr h->r->r

Key ITEMrand(void);
int ITEMscan(Key*);
void ITEMshow(Item);

void ITEMshow(Item x) { printf("%3d ", key(x)); }

#define M 10001
#define ws (M + 1)

#define l 3  // max = 9
#define min(A, B, C) (A > B ? (B > C ? C : B) : (A > C ? C : A))

typedef struct STnode* link;
typedef struct leaf* link_leaf;
typedef struct {
    Key key;
    union {
        link next;
        struct leaf {
            Item item;
            link_leaf next_leaf;  // 葉ノード同士を接続するためのポインタを追加
        } leaf;
    } ref;
} entry;

/*typedef struct {
    Key key;
    union {
        link next;
        Item item;
    } ref;
} entry;*/

struct STnode {
    entry b[ws];
    int m;
};
static link head;
static int H, n;

link NEW() {
    link x = malloc(sizeof *x);
    x->m = 0;
    return x;
}
void STinit(int maxN) {
    head = NEW();
    H = 0;
    n = 0;
}

link split(link h) {
    int j;
    link t = NEW();
    for (j = 0; j < ws / 2; j++) t->b[j] = h->b[ws / 2 + j];
    h->m = ws / 2;
    t->m = ws / 2;
    return t;
}

link_leaf searchR(link h, Key v, int H) {
    int j;
    if (H == 0)
        for (j = 0; j < h->m; j++)
            if (eq(v, h->b[j].key)) return &(h->b[j].ref.leaf);
    if (H != 0)
        for (j = 0; j < h->m; j++)
            // ページの全てが同じキーで埋まってしまっている場合に
            // 一番先頭の要素を検索できるように条件を追加
            if ((j + 1 == h->m) || less(v, h->b[j + 1].key || v == h->b[j].key))
                return searchR(h->b[j].ref.next, v, H - 1);
    return NULL;  //?
}

link_leaf STsearch(Key v) { return searchR(head, v, H); }

link insertR(link h, Item item, int H) {
    int i, j;
    Key v = key(item);
    entry x;
    link t, u;
    x.key = v;
    x.ref.leaf.item = item;
    if (H == 0)
        for (j = 0; j < h->m; j++)
            if (less(v, h->b[j].key)) break;
    if (H != 0)
        for (j = 0; j < h->m; j++)
            if ((j + 1 == h->m) || less(v, h->b[j + 1].key)) {
                t = h->b[j++].ref.next;
                u = insertR(t, item, H - 1);
                if (u == NULL) return NULL;
                x.key = u->b[0].key;
                x.ref.next = u;
                break;
            }
    for (i = (h->m)++; i > j; i--) h->b[i] = h->b[i - 1];
    h->b[j] = x;
    if (h->m < ws)
        return NULL;
    else
        return split(h);
}

void STinsert(Item item) {
    link t, u = insertR(head, item, H);
    if (u == NULL) return;
    t = NEW();
    t->m = 2;
    t->b[0].key = head->b[0].key;
    t->b[0].ref.next = head;
    t->b[1].key = u->b[0].key;
    t->b[1].ref.next = u;
    head = t;
    H++;
}

void STshow(link h, int H) {
    int i;
    if (H == 0) {
        for (i = 0; i < h->m; i++) {
            if (i == 0) ITEMshow(h->b[i].ref.leaf.item);
        }
        printf("| ");
    } else {
        for (i = 0; i < h->m; i++) {
            STshow(h->b[i].ref.next, H - 1);
        }
    }
}

void STshowAll() {
    STshow(head, H);
    printf("\n");
}

link_leaf tmp1 = NULL;

void add_linkR(link h, int H) {
    int i;
    if (H == 0) {
        for (i = 0; i < h->m; i++) {
            if (tmp1 != NULL) {
                tmp1->next_leaf = &(h->b[i].ref.leaf);
            }
            tmp1 = &(h->b[i].ref.leaf);
        }
    } else {
        for (i = 0; i < h->m; i++) {
            add_linkR(h->b[i].ref.next, H - 1);
        }
    }
}

void add_linkAll() {
    add_linkR(head, H);
    tmp1->next_leaf = NULL;
}  //?

// 葉ノード間のリンクが付与できているかtest
void STtest(link_leaf h) {
    if (h != NULL) {
        while (1) {
            ITEMshow(h->item);
            h = h->next_leaf;

            if (h == NULL) {
                printf("END STtest\n");
                break;
            }
        }
    } else {
        printf("h is NULL");
    }
}

void STtestAll() {
    link_leaf ll = STsearch(0);
    STtest(ll);
    printf("\n");
}

// from https://ohnishiakira.hatenadiary.org/entry/20090809/1249845529
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
    /*printf("edit_dis bw %s & %s : %d\n", str1, str2,
           d[lenstr1 - 1][lenstr2 - 1]);*/
    return d[lenstr1 - 1][lenstr2 - 1];
}

// 前処理
void split_and_insert(char* s, int c) {
    Item item;
    for (int i = 0; i < DATA_LENGTH; i += l) {
        char sub_s[l + 1];
        strncpy(sub_s, s + i, l);
        sub_s[l] = '\0';
        // printf("%s\n", sub_s);
        key(item) = atoi(sub_s);
        item.channel = c;
        item.start = i;
        STinsert(item);
    }
}

// 探索
// t = Threshold(閾値)
int split_and_search(char** S, char* q, int t) {
    link_leaf ll;
    for (int i = 0; i < l - l + 1; i++) {  // offset毎の木を作ってから
        for (int j = 0; j < strlen(q); j += l) {
            char sub_q[l + 1];
            strncpy(sub_q, q + j + i, l);
            sub_q[l] = '\0';
            ll = STsearch(atoi(sub_q));
            // printf("sub_q:%s\n", sub_q);

            // 完全一致した全ての要素について，編集距離を算出
            while (1) {
                if (ll != NULL) {
                    // join(item, j + o);
                    int edit_dis;
                    char sub_s_i[strlen(q)];
                    int offset = ll->item.start - j - i;
                    if (offset < 0 && ll->next_leaf != NULL &&
                        ll->item.key == ll->next_leaf->item.key) {
                        ll = ll->next_leaf;
                        continue;
                    }
                    strncpy(sub_s_i, S[ll->item.channel] + offset, strlen(q));
                    sub_s_i[strlen(q)] = '\0';
                    edit_dis = calc_edit_dis(sub_s_i, q);
                    if (edit_dis <= t) {
                        printf("edit_dis_best:%d, ans:%s\n", edit_dis, sub_s_i);
                        return ll->item.channel + 1;
                    }
                } else {  //(無いとは思うけど)B木内に一致する部分列が一つもないとき
                    // printf("No matching subsequence in B-tree\n");
                    break;
                }
                if (ll->next_leaf != NULL &&
                    ll->item.key == ll->next_leaf->item.key) {
                    ll = ll->next_leaf;
                } else {
                    // printf("Break!\n");
                    break;
                }
            }
        }
    }
    return 102;
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));
    printf("%s\n", argv[1]);
    FILE* input_file = fopen(argv[1], "r");
    FILE* output_file = fopen(argv[2], "w");
    FILE* answer_file = fopen(argv[3], "r");

    if (input_file == NULL || output_file == NULL || answer_file == NULL) {
        fprintf(stderr, "error\n");
        exit(EXIT_FAILURE);
    }

    int p_ins, p_sub, p_del;
    fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);
    float eps = 0.1;
    float threshold_ratio =
        1.0 -
        (float)((100 - p_ins) * (100 - p_ins) * (100 - p_ins)) / 1000000.0f +
        eps;

    STinit(N * (DATA_LENGTH / l));

    char** S = (char**)malloc(sizeof(char*) * N);
    // S[i] に基地局 i
    // の情報データを格納する (問題では，1<=i<=100だが，本プログラムでは，
    // 簡単のため0<=i<=99として計算を行い，出力時に+1する予定)
    for (int i = 0; i < N; i++) {
        S[i] = (char*)malloc(sizeof(char) * (DATA_LENGTH + 1));
        fscanf(input_file, "%s", S[i]);
        split_and_insert(S[i], i);
        // printf("%s\n", s_i);
    }

    add_linkAll();
    // STshowAll();
    // STtestAll();
    printf("Finished building B-tree\n");

    for (int i = 0; i < Q; i++) {
        char* q = (char*)malloc(sizeof(char) * 200);
        fscanf(input_file, "%s", q);
        q = ask(i + 1, argv[3]);
        printf("%d query:%s\n", i + 1, q);

        int distance_threshold = (int)((1.0 - 0.729 + 0.05) * strlen(q));
        int ans = split_and_search(S, q, distance_threshold);
        printf("Ans:%d, distance_threshold:%d\n", ans, distance_threshold);
        // int ans = randint(1, N + 1);
        fprintf(output_file, "%d\n", ans);

        free(q);
    }

    fclose(input_file);
    fclose(output_file);
    for (int i = 0; i < N; i++) {
        free(S[i]);
    }
    free(S);

    return 0;
}