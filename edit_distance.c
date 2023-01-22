#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

int min(int a, int b, int c){
	return a > b ? (b > c ? c : b) : (a > c ? c : a);
}

int levenshtein(char *str1, char *str2){
	int lenstr1 = strlen(str1) + 1;
	int lenstr2 = strlen(str2) + 1;
	int d[lenstr1][lenstr2];
	int i1 = 0, i2 = 0, cost = 0;

	for (;i1 < lenstr1; i1++) d[i1][0] = i1;
	for (;i2 < lenstr2; i2++) d[0][i2] = i2;

	for (i1 = 1; i1 < lenstr1; i1++) {
		for (i2 = 1; i2 < lenstr2; i2++) {
			cost = str1[i1 - 1] == str2[i2 - 1] ? 0 : 1;
			d[i1][i2] = min(d[i1 - 1][i2] + 1, d[i1][i2 - 1] + 1, d[i1 - 1][i2 - 1] + cost);
		}
	}
	return d[lenstr1 - 1][lenstr2 - 1];
}

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

char* randomString(int len) {
    char *str = malloc(len + 1);
    int i;
    for (i = 0; i < len; i++) {
        str[i] = '0' + (rand() % 4);
    }
    str[len] = '\0';
    return str;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  int dist1, dist2;
  clock_t time1, time2, time3, time4;

  printf("Edit Distance\t\t\tTime\nnormal\t64-bitparallel\t\tnormal\t64-bitparallel\n");
  for(int i = 0; i < 10; i++) {
    char *a = randomString((rand() % 64) + 1);
    char *b = randomString((rand() % 64) + 1);

    time1 = clock();
    dist1 = levenshtein(a, b);
    time2 = clock();

    time3 = clock();
    dist2 = levenshtein_bitpal64(a, strlen(a), b, strlen(b));
    time4 = clock();

    printf("%d\t%d", dist1, dist2);
    if (dist1 != dist2) {
      printf("\tmiss!\t\t");
    } else {
      printf("\t\t\t");
    }
    printf("%ld\t%ld\n", time2 - time1, time4 - time3);

    free(a);
    free(b);
  }
  printf("\n");
  
  printf("Edit Distance\t\t\tTime\nnormal\t128-bitparallel\t\tnormal\t128-bitparallel\n");
  for(int i = 0; i < 10; i++) {
    char *a = randomString((rand() % 64) + 65);
    char *b = randomString((rand() % 64) + 65);

    time1 = clock();
    dist1 = levenshtein(a, b);
    time2 = clock();

    time3 = clock();
    dist2 = levenshtein_bitpal128(a, strlen(a), b, strlen(b));
    time4 = clock();

    printf("%d\t%d", dist1, dist2);
    if (dist1 != dist2) {
      printf("\tmiss!\t\t");
    } else {
      printf("\t\t\t");
    }
    printf("%ld\t%ld\n", time2 - time1, time4 - time3);

    free(a);
    free(b);
  }

  return 0;
}