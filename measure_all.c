// コンパイル: gcc -o measure_all measure_all.c 
// 実行: ./measure_all run_G2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]){
    int number, i, j;
    double cpu_time;
    char command[200];
    char tmp[100];
    char user_time[100];
    char sys_time[100];
    char mrss[100];
    char mrss_n[100];
    char idata_path[100];
    char answer_path[100];
    double sum_cpu_time = 0.0;
    int sum_mrss = 0;
    int sum_score = 0;
    int atoi_mrss, score;
    
    FILE *result_file = fopen("measurement_result.txt", "w");

    for(number=1;;number++){
        memset(idata_path, '\0', sizeof(idata_path));
        memset(answer_path, '\0', sizeof(answer_path));

        sprintf(idata_path, "all/testcase%d/idata", number);
        FILE* check_i = fopen(idata_path, "r");
        if (check_i == NULL) {
            break;
        }
        fclose(check_i);
        sprintf(answer_path, "all/testcase%d/answer", number);
        FILE* check_a = fopen(answer_path, "r");
        if (check_a == NULL) {
            break;
        }
        fclose(check_a);
        
        memset(command, '\0', sizeof(command));
        memset(tmp, '\0', sizeof(tmp));
        memset(user_time, '\0', sizeof(user_time));
        memset(sys_time, '\0', sizeof(sys_time));
        memset(mrss, '\0', sizeof(mrss));
        memset(mrss_n, '\0', sizeof(mrss));

        //timeコマンド実行
        sprintf(command, "/usr/bin/time -o info.txt -v ./%s %s out.txt %s 1> /dev/null 2> ask.txt", argv[1], idata_path, answer_path);
        system(command);

        //timeコマンドの出力を取得
        FILE* info_file = fopen("info.txt", "r");
        for (i = 0; i < 2; i++) {
            fgets(tmp, 100, info_file);
        }
        for(i=22; tmp[i+1]!='\0'; i++){
            user_time[i-22] = tmp[i];
        }

        fgets(tmp, 100, info_file);
        for(i=24; tmp[i+1]!='\0'; i++){
            sys_time[i-24] = tmp[i];
        }
        
        for (i = 0; i < 7; i++) {
            fgets(mrss, 200, info_file);
        }
        fclose(info_file);        

        printf("\nテストケース%d\n", number);
        fprintf(result_file, "\nテストケース%d\n", number);

        //CPU時間
        cpu_time = atof(user_time) + atof(sys_time);

        //ピークメモリ
        i = 0;
        j = 0;
        while(mrss[i]!='\0'){
            if(mrss[i]>='0' && mrss[i]<='9'){
                mrss_n[j] = mrss[i];
                j++;
            }
        i++;
        }
        atoi_mrss = atoi(mrss_n);
        sum_mrss += atoi_mrss;

        //スコア採点
        //ask回数
        char query[4];
        int ask[100];
        int n;
        int lost_score = 0;
        for(i=0; i<100; i++){
            ask[i] = 0;
        }
        FILE* ask_file = fopen("ask.txt", "r");
        while(fgets(tmp, 200, ask_file) != NULL){
            if(tmp[0] == 'a'){
                i = 0;
                j = 0;
                while(tmp[i]!='\0'){
                    if(tmp[i]>='0' && tmp[i]<='9'){
                        query[j] = tmp[i];
                        j++;
                    }
                i++;
                }
                n = atoi(query) - 1;
                if(ask[n] < 18){ //正解時の100ポイントに対するaskによる減点は90ポイントまで
                    ask[n]++;
                }
            }
        }
        fclose(ask_file);

        //正答数
        FILE* output_file = fopen("out.txt", "r");
        FILE* answer_file = fopen(answer_path, "r");
        int count_correct = 0;
        int out;
        int answer;
        int ask_and_fail = 0;
        fgets(tmp, 200, answer_file);
        for (i = 0; i < 100; i++) {
            fscanf(output_file, "%d", &out);
            fscanf(answer_file, "%d %s", &answer, tmp);
            if (out == answer) {
                count_correct++;
            }else{
                ask_and_fail += ask[i]*5; // 不正解時の0ポイントに対するaskによる減点は0ポイントのため、正解したものとして減点したポイントを後で戻す
            }
        }
        fclose(output_file);
        fclose(answer_file);

        for(i=0; i<100; i++){
            lost_score += 5 * ask[i];
        }
        if(cpu_time > 10.0){
            printf(" CPU時間: %.2f[s](上限越え, 10[s]とする)\n", cpu_time);
            fprintf(result_file, " CPU時間: %.2f[s](上限越え, 10[s]とする)\n", cpu_time);
            cpu_time = 10.0;
            printf(" ピークメモリ: %d[kB]\n", atoi_mrss);
            fprintf(result_file, " ピークメモリ: %d[kB]\n", atoi_mrss);
            score = 0;
            printf(" スコア: %d/10000(10秒オーバー)\n", score);
            fprintf(result_file, " スコア: %d/10000(10秒オーバー)\n", score);
        }else{
            printf(" CPU時間: %.2f[s]\n", cpu_time);
            fprintf(result_file, " CPU時間: %.2f[s]\n", cpu_time);
            printf(" ピークメモリ: %d[kB]\n", atoi_mrss);
            fprintf(result_file, " ピークメモリ: %d[kB]\n", atoi_mrss);
            score = 100*count_correct - lost_score + ask_and_fail;
            printf(" スコア: %d/10000\n", score);
            fprintf(result_file, " スコア: %d/10000\n", score);
        }
        printf(" ├正答数: %d/100\n", count_correct);
        printf(" └ask回数: %d\n", lost_score/5);
        fprintf(result_file, " ├正答数: %d/100\n", count_correct);
        fprintf(result_file, " └ask回数: %d\n", lost_score/5);
        sum_score += score;
        sum_cpu_time += cpu_time;
    }
    printf("\n平均CPU時間: %.2f[s]\n", sum_cpu_time/(number-1));
    printf("平均ピークメモリ: %d[kB]\n", sum_mrss/(number-1));
    printf("合計スコア: %d/%d\n", sum_score, 10000*(number-1));
    printf("合計スコア割合: %.2f%%\n", (double)sum_score/(100*(number-1)));
    fprintf(result_file, "\n平均CPU時間: %.2f[s]\n", sum_cpu_time/(number-1));
    fprintf(result_file, "平均ピークメモリ: %d[kB]\n", sum_mrss/(number-1));
    fprintf(result_file, "合計スコア: %d/%d\n", sum_score, 10000*(number-1));
    fprintf(result_file, "合計スコア割合: %.2f%%\n", (double)sum_score/(100*(number-1)));
    fclose(result_file);
    return 0;
}