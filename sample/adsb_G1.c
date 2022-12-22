#include <stdio.h>
#include <stdlib.h>

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

Item NULLitem = {-1};

Key ITEMrand(void);
int ITEMscan(Key*);
void ITEMshow(Item);

void ITEMshow(Item x) { printf("%3d ", key(x)); }

#define M 5
#define ws (M + 1)

typedef struct STnode* link;
typedef struct {
    Key key;
    union {
        link next;
        Item item;
    } ref;
} entry;

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

Item searchR(link h, Key v, int H) {
    int j;
    if (H == 0)
        for (j = 0; j < h->m; j++)
            if (eq(v, h->b[j].key)) return h->b[j].ref.item;
    if (H != 0)
        for (j = 0; j < h->m; j++)
            // ページの全てが同じキーで埋まってしまっている場合に
            // 一番先頭の要素を検索できるように条件を追加
            if ((j + 1 == h->m) || less(v, h->b[j + 1].key || v == h->b[j].key))
                return searchR(h->b[j].ref.next, v, H - 1);
    return NULLitem;
}

Item STsearch(Key v) { return searchR(head, v, H); }

link insertR(link h, Item item, int H) {
    int i, j;
    Key v = key(item);
    entry x;
    link t, u;
    x.key = v;
    x.ref.item = item;
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
            ITEMshow(h->b[i].ref.item);
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

#define l 9
#define min(A, B, C) (A > B ? (B > C ? C : B) : (A > C ? C : A))

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

void add_link() {}

void split_and_search(char* q, int c, int o) {
    Item item;
    for (int i = 0; i < DATA_LENGTH; i += l) {
        char sub_q[l + 1];
        strncpy(sub_q, q + i + o, l);
        sub_q[l] = '\0';
        item = STsearch(atoi(sub_q));
        if (item != NULLitem) {
            // join(item, i + o);
        }
    }
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

    if (input_file == NULL || output_file == NULL || answer_file == NULL) {
        fprintf(stderr, "error\n");
        exit(EXIT_FAILURE);
    }

    int p_ins, p_sub, p_del;
    fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);

    STinit(N * (DATA_LENGTH / l));

    char** S = (char**)malloc(sizeof(char*) * N);
    // S[i] に基地局 i の情報データを格納する

    for (int i = 0; i < N; i++) {
        S[i] = (char*)malloc(sizeof(char) * (DATA_LENGTH + 1));
        fscanf(input_file, "%s", S[i]);
        split_and_insert(S[i], i);
        // printf("%s\n", s_i);
    }

    // STshowAll();

    for (int i = 0; i < Q; i++) {
        char* q = (char*)malloc(sizeof(char) * 200);
        fscanf(input_file, "%s", q);
        // q = ask(i + 1, argv[3]); how to ask

        int j = atoi(q);
        printf("%d\n", j);
        int ans = randint(1, N + 1);
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