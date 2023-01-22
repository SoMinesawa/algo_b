#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


int levenshtein_bitpal64(char* a, int len_a, char* b, int len_b) {
    if (len_a > 64) {
      return -1;
    }

    uint64_t posbits[256] = {0};

    for (int i=0; i < len_a; i++){
        posbits[(unsigned char)a[i]] |= 1ull << i;
    }  
    
    int dist = len_a;
    uint64_t mask = 1ull << (len_a - 1);
    uint64_t VP = 0xffffffffffffffff >> (64 - len_a);
    uint64_t VN   = 0;

    for (int i=0; i < len_b; i++){
      uint64_t y = posbits[(unsigned char)b[i]];
      uint64_t X  = y | VN; 
      uint64_t D0 = ((VP + (X & VP)) ^ VP) | X;
      uint64_t HN = VP & D0;
      uint64_t HP = VN | ~(VP|D0);
      X  = (HP << 1) | 1;
      VN =  X & D0;
      VP = (HN << 1) | ~(X | D0);
      if (HP & mask) { dist++; }
      if (HN & mask) { dist--; }
    }
    return dist; 
}


typedef struct {
    uint64_t high;
    uint64_t low;
} uint128_t;

// 128ビット整数加算関数
uint128_t add_uint128(uint128_t a, uint128_t b) {
    uint128_t result;
    result.low = a.low + b.low;
    result.high = a.high + b.high;
    if (result.low < a.low) {
        result.high++;
    }
    return result;
}

// 128ビット整数論理積
uint128_t and_uint128(uint128_t a, uint128_t b) {
    uint128_t result;
    result.high = a.high & b.high;
    result.low = a.low & b.low;
    return result;
}

// 128ビット整数論理和
uint128_t or_uint128(uint128_t a, uint128_t b) {
    uint128_t result;
    result.high = a.high | b.high;
    result.low = a.low | b.low;
    return result;
}

// 128ビット整数反転
uint128_t not_uint128(uint128_t a) {
    uint128_t result;
    result.high = ~a.high;
    result.low = ~a.low;
    return result;
}

// 128ビット整数排他的論理和
uint128_t xor_uint128(uint128_t a, uint128_t b) {
    uint128_t result;
    result.high = a.high ^ b.high;
    result.low = a.low ^ b.low;
    return result;
}

// 128ビット整数左シフト
uint128_t shl_uint128(uint128_t a, int b) {
    uint128_t result;
    if (b >= 64) {
        result.high = a.low << (b - 64);
        result.low = 0;
    } else {
        result.high = (a.high << b) | (a.low >> (64 - b));
        result.low = a.low << b;
    }
    return result;
}

uint128_t shr_uint128(uint128_t x, int n) {
  if (n >= 64) {
    x.low = x.high >> (n - 64);
    x.high = 0;
  } else {
    x.low = (x.high << (64 - n)) | (x.low >> n);
    x.high >>= n;
  }
  return x;
}

// どこか間違ってる 実際の編集距離より1小さいものを返す場合がある
int levenshtein_bitpal128(char* a, int len_a, char* b, int len_b) {
    if (len_a > 128) {
      return -1;
    }
    uint128_t one = {0, 1ull};
    uint128_t max = {UINT64_MAX, UINT64_MAX};
    uint128_t posbits[256] = {{0, 0}};
    for (int i=0; i < len_a; i++){
        posbits[(unsigned char)a[i]] = or_uint128(posbits[(unsigned char)a[i]], shl_uint128(one, i));
    }  
    int dist = len_a;
    uint128_t mask = shl_uint128(one, len_a - 1);
    uint128_t VP = shr_uint128(max, 128 - len_a);
    uint128_t VN = {0, 0};
    for (int i=0; i < len_b; i++){
      uint128_t y = posbits[(unsigned char)b[i]]; 
      uint128_t X = or_uint128(y, VN);
      uint128_t D0 = or_uint128(xor_uint128(add_uint128(VP, and_uint128(X, VP)), VP), X);
      uint128_t HN = and_uint128(VP, D0);
      uint128_t HP = or_uint128(VN, not_uint128(or_uint128(VP, D0)));
      X = or_uint128(shl_uint128(HP, 1), one);
      VN = and_uint128(X, D0);
      VP = or_uint128(shl_uint128(HN, 1), not_uint128(or_uint128(X, D0)));
      uint128_t HP_and_mask = and_uint128(HP, mask);
      if (HP_and_mask.high != 0 || HP_and_mask.low != 0) {dist++;}
      uint128_t HN_and_mask = and_uint128(HN, mask);
      if (HN_and_mask.high != 0 || HN_and_mask.low != 0) {dist--;}
    }
    return dist; 
}

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

char* randomString(int len) {
    char *str = malloc(len + 1);
    for(int i = 0; i < len; i++) {
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