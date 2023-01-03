/**
 * @file adsb_G1.c
 * @author So Minesawa, Ide Kohsuke, So Fukuda, Ryosuke Nutaba, Shun Kawai
 * @brief Implementation of an efficient solution method for group assignments
 * using B+ trees
 * @version 0.1
 * @date 2022-12-29
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ask.h"
#include <sys/time.h>

/**
 * @brief
 * B+木の実装に必要なマクロなどの定義
 * (基本的には講義のコードを引用しているため，適宜参照することを推奨する)
 */

/** B+木の次数 M */
#define M 51
#define ws (M + 1)

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

#define min(A, B, C) (A > B ? (B > C ? C : B) : (A > C ? C : A))

typedef struct STnode* link;
typedef struct leaf* link_leaf;

/**
 * B+木に特有な，外部節点間を接続するリンクを実装するため，
 * 共用体 ref の中身を変更している．
 * 外部節点である場合には，共用体 ref は leaf となり，Item と
 * 次の外部節点へのリンク link_leaf を持つ．
 */
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

struct STnode {
    entry b[ws];
    int m;
};

/**B+木を複数扱いたいため，B+木を表す構造体 BPTree を用意*/
typedef struct {
    link head;
    int H;
    int n;
} BPTree;

link NEW() {
    link x = malloc(sizeof *x);
    x->m = 0;
    return x;
}
void STinit(BPTree* bp, int maxN) {
    bp->head = NEW();
    bp->H = 0;
    bp->n = 0;
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
    return NULL;
}

/**
 * @brief
 * 値 v をB+木の中から探索する．値 v
 * が複数ある場合には，先行順で最初に見つかるものを返す．
 * @param lbp 探索したいB+木の根のポインタ
 * @param v 探索する値
 * @return link_leaf 値 v の持つ外部節点 leaf
 * の内，先行順で最初に見つかるもののポインタ
 */
link_leaf STsearch(BPTree* lbp, Key v) { return searchR(lbp->head, v, lbp->H); }

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

void STinsert(BPTree* lbp, Item item) {
    link t, u = insertR(lbp->head, item, lbp->H);
    if (u == NULL) return;
    t = NEW();
    t->m = 2;
    t->b[0].key = lbp->head->b[0].key;
    t->b[0].ref.next = lbp->head;
    t->b[1].key = u->b[0].key;
    t->b[1].ref.next = u;
    lbp->head = t;
    lbp->H++;
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

void STshowAll(BPTree bp) {
    STshow(bp.head, bp.H);
    printf("\n");
}

/** 関数 add_linkR 内で用いるグローバル変数 */
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

/**
 * @brief
 * 与えられたB+木の全ての外部節点間に対して，一方向のリンクをつける．
 * @param lbp B+木の根のポインタ(まだ外部節点間のリンクが無いもの)
 */
void add_linkAll(BPTree* lbp) {
    add_linkR(lbp->head, lbp->H);
    tmp1->next_leaf = NULL;
}

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
        printf("h is NULL\n");
    }
}

/**
 * @brief 外部節点間のリンクが想定通りに与できているかテストする
 *
 * @param lbp B+木の根のポインタ
 */
void STtestAll(BPTree* lbp) {
    link_leaf ll = STsearch(lbp, 0);
    STtest(ll);
    printf("\n");
}

/**
 * @brief
 * str1 と str2 の編集距離を計算
 * from https://ohnishiakira.hatenadiary.org/entry/20090809/1249845529
 * @param str1
 * @param str2
 * @return int
 */
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

/**
 * @brief
 * 文字列を長さ l で分割し，B+木に格納する
 * @param b 格納するB+木のポインタ
 * @param s 格納する文字列 (分割前)
 * @param c 文字列 s の基地局番号
 * @param l 分割の長さ l
 */
void insert_tree(BPTree* b, char* s, int c, int l) {
    Item item;
    for (int i = 0; i < l;
         i++) {  // 文字列 s の区切り初め(offset)を一文字ずつずらす
        for (int j = 0; i + j + l <= DATA_LENGTH; j += l) {
            // 文字列 s を分割の長さ l で分割していく
            char* sub_s = slice(s, i + j, l);

            // 分割した文字列をB+木に格納する
            key(item) = atoi(sub_s);
            item.channel = c;
            item.start = i + j;
            STinsert(&(b[i]), item);
            free(sub_s);
        }
    }
}

/**
 * @brief
 * 与えられたクエリ q から正解の基地局を求める関数．
 * クエリ q を分割した sub_q をB+木中から探索 →
 * ヒットした候補と q との編集距離を算出 → 閾値より小さければ答え
 * @param lbp B+木の配列のポインタ
 * @param S 正解データ S
 * @param q クエリ q
 * @param t 編集距離の閾値 t
 * @param l 分割の長さ l
 * @return int 正解の基地局
 */
int search(BPTree* lbp, char** S, char* q, int t, int l) {
    link_leaf ll;
    int edit_dis;
    for (int i = 0; i < l; i++) {  // offsetを1ずつずらす
        for (int j = 0; i + j + l <= strlen(q); j += l) {
            // クエリ文字列 q を分割の長さ l で分割していく
            char* sub_q = slice(q, i + j, l);
            // sub_qをB+木から探索
            for (int k = 0; k < l; k++) {
                ll = STsearch(&lbp[k], atoi(sub_q));
                // printf("sub_q:%s\n", sub_q);

                if (ll == NULL) {  // B+木内に一致する部分列が一つもないとき
                    // printf("No matching subsequence in B-tree\n");
                    continue;
                }

                // B+の内，完全一致した全ての要素について編集距離を算出し，閾値と比較
                while (1) {
                    // sub_q とデータ S との比較を行うため，Sのスライスを行う
                    // offset はデータ S のスライスの開始位置
                    int offset = ll->item.start - j - i;
                    if (offset >= 0) {
                        char* sub_s =
                            slice(S[ll->item.channel], offset, strlen(q));

                        // スライスした S とクエリ q との編集距離を算出
                        edit_dis = calc_edit_dis(sub_s, q);
                        free(sub_s);
                        if (edit_dis <= t) {  // 編集距離が閾値以下であれば
                            return ll->item.channel + 1;
                        }
                    }
                    // 値が同じである次の外部節点に移る
                    if (ll->next_leaf != NULL &&
                        ll->item.key == ll->next_leaf->item.key) {
                        ll = ll->next_leaf;
                    } else {  // 外部節点の値が変わったら while 終了
                        break;
                    }
                }
            }
            free(sub_q);
        }
    }
    // printf("It was not found in (almost) Brute-force search! Return
    // RANDOM!!!\n");
    //  編集距離が最小値の値返すのもあり
    return randint(1, N + 1);
}

// 1個のテストケースに対して1個の出力ファイルを！
int test(float t, int l, char* argv1, char* argv3) {
    FILE* input_file = fopen(argv1, "r");
    FILE* answer_file = fopen(argv3, "r");

    if (input_file == NULL || answer_file == NULL) {
        fprintf(stderr, "error\n");
        exit(EXIT_FAILURE);
    }

    int p_ins, p_sub, p_del;
    fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);
    fscanf(answer_file, "%d %d %d", &p_ins, &p_sub, &p_del);
    // fclose(input_file);  // reset

    /**askの回数カウンタ*/
    int count_ask = 0;
    int score = 0;
    int ans;
    char tmp[200];

    /**B+木の l 個の配列*/
    BPTree* bp_trees = (BPTree*)malloc(sizeof(BPTree) * l);
    for (int i = 0; i < l; i++) {
        STinit(&bp_trees[i], N * (DATA_LENGTH / l));
    }

    /**
     * 前処理 (B+木の構築)
     * S[i] に基地局 i の情報データを格納し，それをB+木に格納する
     * (問題では，1<=i<=100だが，本プログラムでは，
     * 簡単のため0<=i<=99として計算を行い，出力時に+1する予定)
     */
    char** S = (char**)malloc(sizeof(char*) * N);
    for (int i = 0; i < N; i++) {
        S[i] = scanf_from_file(input_file, DATA_LENGTH + 1);
        insert_tree(bp_trees, S[i], i, l);
    }

    // B+木の外部節点間のリンクを接続
    for (int i = 0; i < l; i++) {
        add_linkAll(&bp_trees[i]);
    }
    // printf("Finish costruct b+tree\n");

    // ↑↑↑　前処理終わり
    // ↓↓↓　探索開始
    for (int i = 0; i < Q; i++) {
        char* q = scanf_from_file(input_file, 200);
        // q = ask(i + 1, argv[3]); memo : how to call ask method
        // printf("%d query:%s\n", i + 1, q);

        int distance_threshold = (int)(t * strlen(q));

        int result = search(bp_trees, S, q, distance_threshold, l);

        fscanf(answer_file, "%d %s", &ans, &tmp);
        // printf("Result:%d, Answer:%d, distance_threshold:%d\n", result,
        // ans,distance_threshold);
        if (result == ans) score += 100;
        free(q);
    }
    // ↑↑↑　探索終わり

    for (int i = 0; i < N; i++) {
        free(S[i]);
    }
    free(S);
    free(bp_trees);

    fclose(input_file);
    return score;
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));
    unsigned int sec;
    int nsec;
    double d_sec;
    struct timespec start_time, end_time;
    // printf("%s,%s,%s,%s\n", argv[0], argv[1], argv[2], argv[3]);
    int p = atoi(argv[3]);

    float t_min = 0.20, t_max = 0.50, t_iv = 0.01;
    // float t_min = 1.0 - 0.729 + 0.05, t_max = 1.0 - 0.729 + 0.06,
    // t_iv = 0.01; test paras
    int l_min = 3, l_max = 9, l_iv = 1;
    int count = 0;

    int p_ins, p_sub, p_del;

    p_ins = p / 3;
    p_sub = p_ins;
    p_del = p - p_ins - p_sub;
    if (p_del == 10) {
        p_del--;
        p_sub++;
    }
    FILE* output_file = fopen(argv[2], "a");
    char input_path[256];
    char answer_path[256];

    sprintf(input_path, "%s/idata_%d%d%d", argv[1], p_ins, p_sub, p_del);
    sprintf(answer_path, "%s/answer_%d%d%d", argv[1], p_ins, p_sub, p_del);

    //
    for (float t = t_min; t <= t_max; t += t_iv) {
        for (int l = l_min; l <= l_max; l += l_iv) {
            clock_gettime(CLOCK_REALTIME, &start_time);
            int score = test(t, l, input_path, answer_path);
            clock_gettime(CLOCK_REALTIME, &end_time);
            sec = end_time.tv_sec - start_time.tv_sec;
            nsec = end_time.tv_nsec - start_time.tv_nsec;
            d_sec = (double)sec + (double)nsec / (1000 * 1000 * 1000);

            fprintf(output_file, "%d,%d,%d,%d,%.2f,%d,%d,%.2f\n", p_ins, p_sub,
                    p_del, p, t, l, score, d_sec);

            count++;
            printf("%.2f\%\n",
                   (float)((count)*100.0) / (((t_max - t_min + t_iv) / t_iv) *
                                             ((l_max - l_min + l_iv) / l_iv)));
        }
    }

    fclose(output_file);
    return 0;
}