// コンパイル: gcc -o measure_all measure_all.c 
// 実行: ./measure_all run_G2

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    int testcase;
    double sum_cpu_time = 0;
    double sum_mrss = 0;
    int sum_score = 0;
    for (testcase = 1; ; testcase++) {
        // テストケースに対応するファイルが存在するか
        char idata_path[100];
        sprintf(idata_path, "all/testcase%d/idata", testcase);
        FILE* check_i = fopen(idata_path, "r");
        if (check_i == NULL) { break;}
        fclose(check_i);
        char answer_path[100];
        sprintf(answer_path, "all/testcase%d/answer", testcase);
        FILE* check_a = fopen(answer_path, "r");
        if (check_a == NULL) { break;}
        fclose(check_a);

        // コマンド
        char command[300];
        sprintf(command, "/usr/bin/time -o info.txt -v ./%s %s out.txt %s 1> /dev/null 2> ask.txt", argv[1], idata_path, answer_path);
        system(command);
        
        // info.txtからCPU時間とピークメモリを取得
        FILE* info_file = fopen("info.txt", "r");
        char tmp[200];
        fgets(tmp, 200, info_file);
        double usr_time;
        fscanf(info_file, "\tUser time (seconds): %lf\n", &usr_time);
        double sys_time;
        fscanf(info_file, "\tSystem time (seconds): %lf\n", &sys_time);
        double cpu_time = usr_time + sys_time;
        for (int i = 0; i < 6; i++) { fgets(tmp, 200, info_file);}
        int mrss;
        fscanf(info_file, "\tMaximum resident set size (kbytes): %d\n", &mrss);
        fclose(info_file);

        // out.txtから回答を取得
        int out[100] = {-1};
        FILE* output_file = fopen("out.txt", "r");
        for (int i = 0; i < 100; i++) {
            fscanf(output_file, "%d\n", &out[i]);
        }
        fclose(output_file);

        // answerから正答を取得
        int answer[100] = {-1};
        FILE* answer_file = fopen(answer_path, "r");
        fgets(tmp, 200, answer_file);
        for (int i = 0; i < 100; i++) {
            fscanf(answer_file, "%d %s\n", &answer[i], tmp);
        }
        fclose(answer_file);

        // ask.txtから各クエリにおけるask回数を取得
        int ask[100] = {0};
        int ask_query_number;
        FILE* ask_file = fopen("ask.txt", "r");
        while (fscanf(ask_file, "ask from query_id = %d\n", &ask_query_number) != EOF) {
            ask[ask_query_number - 1]++;
        }
        fclose(ask_file);

        for (int i = 0; i < 100; i++) {
            printf("%d,%d,%d\n", out[i], answer[i], ask[i]);
        }

        // 採点
        int score = 0;
        int count_correct = 0;
        int count_ask = 0;
        for (int i = 0; i < 100; i++) {
            if (out[i] == answer[i]) {
                count_correct++;
                if (ask[i] < 19) {
                    score += 100 - 5 * ask[i];
                    count_ask += ask[i];
                } else {
                    score += 10;
                }
            }
        }

        // スコアの表示
        printf("\nテストケース%d\n", testcase);
        if (cpu_time <= 10) {
            printf(" CPU時間: %.2f[s]\n", cpu_time);
            printf(" ピークメモリ: %d[kB]\n", mrss);
            printf(" スコア: %d/10000\n", score);
        } else {
            printf(" CPU時間: %.2f[s](上限越え)\n", cpu_time);
            printf(" ピークメモリ: %d[kB]\n", mrss);
            printf(" スコア: %d/10000 ※制限時間を超えたので0とする\n", score);
            score = 0;
        }
        printf(" ├正答数: %d/100\n", count_correct);
        printf(" └ask回数: %d\n", count_ask);
        sum_cpu_time += cpu_time;
        sum_mrss += mrss;
        sum_score += score;
    }
    testcase--;

    // 集計結果の表示
    printf("\nテストケース%d個の集計結果\n", testcase);
    printf("平均CPU時間: %.2f[s]\n", sum_cpu_time/testcase);
    printf("平均ピークメモリ: %.0f[kB]\n", (double)sum_mrss/testcase);
    printf("合計スコア: %d/%d\n", sum_score, 10000*testcase);
    printf("合計スコア割合: %.2f%%\n", (double)sum_score/(100*testcase));
    return 0;
}