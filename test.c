#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_LENGTH 200
#define GAP_SCORE -5
#define MATCH_SCORE 1
#define MISMATCH_SCORE -3
// run_G19.c, check_sc.c, test.c, allフォルダが同一ディレクトリ内にあるとして設計.
// コマンドライン引数が空の場合,1～100のテストケースをすべて実行.
// 第1引数<第2引数で指定するとその範囲のテストケースを実行.
// 第1引数に-1を入れるとランダムに10個ほどテストケースを選んで実行.
int main(int argc, char *argv[])
{
    int i, sc = 0, j, st, en, ct = 0, l;
    double k, Ave = 0, memory = 0;
    srand((unsigned)time(NULL));
    char s[10000];
    system("gcc adsb_G2.c -o run_G2 -lm -O2\n");  // 実行プログラムのコンパイル(エラーチェック無し)
    system("gcc check_sc.c -o check_sc -lm -O2\n"); // チェック用プログラムのコンパイル(エラーチェック無し)
    st = 1;
    en = 100;
    if (argc != 1)
    { // コマンドライン引数によって動作を変える
        if (atoi(argv[1]) != -1)
        {
            st = atoi(argv[1]);
            en = atoi(argv[2]);
        }
    }
    for (i = st; i <= en; i++)
    { // テストケース1～100までを連続実行,のち評価
        if (argc != 1 && atoi(argv[1]) == -1)
        {
            if (rand() / (RAND_MAX + 1.0) * 9 > 1)
                continue;
        }
        sprintf(s, "/usr/bin/time -v ./run_G2 all/testcase%d/idata all/testcase%d/out.txt all/testcase%d/answer 2>all/testcase%d/error\n", i, i, i, i); // テストケース実行のためのコマンド生成(エラーログはテキストに保存)
        // printf("%s\n",s);
        j = system(s);                                                                                                                                                // プログラム実行
        sprintf(s, "./check_sc all/testcase%d/idata all/testcase%d/out.txt all/testcase%d/answer all/testcase%d/error > all/testcase%d/result.txt\n", i, i, i, i, i); // チェックのためのコマンド生成(画面標準出力をテキストファイルに保存)
        j = system(s);                                                                                                                                                // チェック(check_sc.cを実行)
        FILE *temp = fopen("temp", "r");
        fscanf(temp, "%lf", &k);
        Ave += k;
        fscanf(temp, "%d", &j);
        memory += j;
        fscanf(temp, "%d", &l);
        printf("testcase %d\n", i);
        printf("Score: %d", l);
        printf(" Time: %.2f,", k);
        printf(" Memory: %d\n", j); // 各ケースごとのテスト結果出力
        sc += l;
        fclose(temp);
        ct++;
    }
    printf("Score: %d, Average Time: %.2f, Average Memory(kB): %.2f\n", sc, Ave / ct, memory / ct); // Summaryを出力
    return 0;
}
