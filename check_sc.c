#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 第1～3引数はプログラム実行の引数と同じ.第4引数にエラーログをテキスト出力したものを指定する.
// test.cに結果を渡すためにゴミを外部ファイルに出力.もっとスマートに組みたかった…
int main(int argc, char *argv[])
{
  FILE *output_file = fopen(argv[2], "r");
  FILE *answer_file = fopen(argv[3], "r");
  FILE *temp = fopen("temp", "w");
  int a;
  int ra, pos;
  int rn;
  int er;
  int sub[100];
  char q[200];
  for (int j = 0; j < 100; j++)
  {
    sub[j] = 0;
  }
  FILE *error_file = fopen(argv[4], "r");
  while (fscanf(error_file, "%s", q) != EOF)
  { // ask呼び出し回数をカウント(Commandの文字列(/usr/bin/time -vをつけて実行するとプログラム終了時にこの文字列が最初に出力されるはず…)を見つけるまで探す)
    if (strcmp(q, "Command") == 0)
      break;
    er = atoi(q);
    if (er < 101 && er > 0)
    {
      sub[er - 1] += 5;
    }
  }
  while (fscanf(error_file, "%s", q) != EOF)
  { // CPU時間とPeak Memoryをそれぞれエラーログから引っ張る
    if (strcmp(q, "User") == 0)
    {
      fscanf(error_file, "%s", q);
      fscanf(error_file, "%s", q);
      fscanf(error_file, "%s", q);
      fprintf(temp, "%.2f\n", atof(q));
    }
    else if (strcmp(q, "Maximum") == 0)
    {
      fscanf(error_file, "%s", q);
      fscanf(error_file, "%s", q);
      fscanf(error_file, "%s", q);
      fscanf(error_file, "%s", q);
      fscanf(error_file, "%s", q);
      fprintf(temp, "%d\n", atoi(q));
    }
  }
  fclose(error_file);
  char S[5001];
  char *find;
  int sc = 0;
  int score = 0;//スコアを記録するための変数
  int p_ins, p_sub, p_del, i;
  fscanf(answer_file, "%d %d %d", &p_ins, &p_sub, &p_del);
  printf("ins:%d, sub:%d, del:%d\n", p_ins, p_sub, p_del); // エラー率出力
  for (i = 0; i < 100; i++)
  { // プログラムの出力とanswerに書いてある答えを照合
    if (fscanf(output_file, "%d", &a) == EOF)
      break;
    if (fscanf(answer_file, "%d %s", &ra, q) == EOF)
      break;
    // printf("%d\n",ra);
    if (a == ra)
    { // 出力と答えが一致の場合
      sc++;
      if (100 - sub[i] < 10)
        score += 10; // ask聞き出し回数により減点を計算しスコアに加算
      else
        score += 100 - sub[i];
    }
    else
    { // 不一致の場合(いろいろと情報を出力)
      FILE *input_file = fopen(argv[1], "r");
      fscanf(input_file, "%d %d %d", &p_ins, &p_sub, &p_del);
      for (int j = 0; j < ra; j++)
      {
        if (fscanf(input_file, "%s", S) == EOF)
          break;
      }
      find = strstr(S, q);
      pos = find - S;
      for (int j = 0; j < 101 - ra + i; j++)
      {
        if (fscanf(input_file, "%s", S) == EOF)
          break;
      }
      // alignment(S, q, (int *)strlen(S), (int *)strlen(q));
      // traceback(S,q,(int *)strlen(S),(int *)strlen(q),s2,t2,&rn);
      printf("No.%d is uncorrect. (Your answer: %d, Correct answer: %d Length: %ld Located from %d)\n", i + 1, a, ra, strlen(S), pos);
      /*for(int l=(rn-2);l>=0;l--){
      printf("%c", s2[i]);
  }printf("\n");
  for(int l=(rn-2);l>=0;l--){
      printf("%c", t2[i]);
  }printf("\n");*/
      fclose(input_file);
    }
  }
  printf("Answered:%d, Correct_Answer: %d, Score: %d, Rate:%.3f\n", i, sc, score, score / (double)i / 100); // Summaryを出力
  fclose(answer_file);
  fclose(output_file);
  fprintf(temp, "%d\n", score);
  fclose(temp);
  return 0;
}