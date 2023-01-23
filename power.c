/*
 * Author: Hiroyuki Chishiro
 * License: 2-Clause BSD
 */
#include <stdio.h>

int get_msb(unsigned int n)
{
  int i = 0;

  while (n > 0) {
    n >>= 1;
    i++;
  }
  /* return -1 if a == 0. */
  return i - 1;
}

unsigned int left2right_binary(unsigned int a, unsigned int n)
{
  unsigned int x = 1;
  int i;
  for (i = get_msb(n); i >= 0; i--) {
    x *= x;

    if ((n >> i) & 1) {
      x *= a;
    }
  }
  return x;
}

int main(void)
{
    int a[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    for (int i = 0; i < 5; i++) {
        printf("%d\n", a[left2right_binary(2, i)]);
    }

    return 0;
}