// コンパイル: gcc -o measure measure.c 
// 実行: ./measure ./run_G2 all/testcase1/idata out.txt all/testcase1/answer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]){
    int i, j;
    double cpu_time;
    char command[200];
    char tmp[100];
    char user_time[100];
    char sys_time[100];
    char mrss[100];
    
    //timeコマンド実行
    sprintf(command, "/usr/bin/time -o info.txt -v %s %s %s %s 2> /dev/null 2> ask.txt", argv[1], argv[2],argv[3],argv[4]);
    system(command);
    printf("\n");

    //timeコマンドの出力を取得
    FILE* info_file = fopen("info.txt", "r");
    for (i = 0; i < 2; i++) {
        fgets(tmp, 100, info_file);
    }
    for(i=22; tmp[i+1]!=NULL; i++){
        user_time[i-22] = tmp[i];
    }

    fgets(tmp, 100, info_file);
    for(i=24; tmp[i+1]!=NULL; i++){
        sys_time[i-24] = tmp[i];
    }
    
    for (i = 0; i < 7; i++) {
        fgets(mrss, 200, info_file);
    }
    fclose(info_file);

    //CPU時間
    cpu_time = atof(user_time) + atof(sys_time);
    printf("CPU時間: %.2f[s]\n", cpu_time);

    //ピークメモリ
    printf("ピークメモリ: ");
    i=0;
    while(mrss[i]!='\0'){
        if(mrss[i]>='0' && mrss[i]<='9'){
            printf("%c", mrss[i]);
        }
    i++;
    }
    printf("[kB]\n");

    //スコア採点
    //正答数
    FILE* output_file = fopen(argv[3], "r");
    FILE* answer_file = fopen(argv[4], "r");
    int count_correct = 0;
    int out;
    int answer;
    fgets(tmp, 200, answer_file);
    for (i = 0; i < 100; i++) {
        fscanf(output_file, "%d", &out);
        fscanf(answer_file, "%d %s", &answer, tmp);
        if (out == answer) {
            count_correct++;
        }
    }
    fclose(output_file);
    fclose(answer_file);

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
            if(ask[n] < 18){
                ask[n]++;
            }
        }
    }

    for(i=0; i<100; i++){
        lost_score += 5 * ask[i];
    }
    printf("スコア: %d/10000\n", 100*count_correct - lost_score);
    printf("├正答数: %d/100\n", count_correct);
    printf("└ask回数: %d\n", lost_score/5);
    fclose(ask_file);
    return 0;
}