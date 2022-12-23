#pragma once
#include <sys/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

unsigned int myrand() {
    unsigned char buf[4];
    ssize_t size = getrandom(buf, 4, GRND_NONBLOCK);
    assert(size != -1);

    unsigned int res = 0;
    for(int i = 0; i < 4; i++) {
        unsigned int tmp = 1;
        for(int j = 0; j < i; j++) tmp *= 256;
        tmp *= (unsigned int)buf[i];
        res += tmp;
    }
    return res;
}

// [l, r)
int randint(int l, int r) {
    assert(l < r);
    return (myrand() % (r - l) + l);
}
