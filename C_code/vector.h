#pragma once
#include <stdlib.h>

/**
 * @file vector.h
 * @brief 動的配列を実装したヘッダファイル。C++のstd::vectorの機能の一部を実装。
 */

// vector に格納する要素の型。
typedef char vector_t;

typedef struct {
    int capacity;
    int tail;
    vector_t* dat;
} vector;

// 要素が size 個予め確保された動的配列を作成する。
// 要素は全て 0 で初期化されている
vector makeVector(int size) {
    vector v;
    v.capacity = size;
    v.tail = size - 1;
    v.dat = (vector_t*)calloc(v.capacity, sizeof(vector_t));
    return v;
}

// vector に格納されている要素の個数を返す。
int getVectorSize(vector* v) {
    return v->tail + 1;
}

// vector で確保されているメモリを開放する
void freeVector(vector* v) {
    free(v->dat);
    free(v);
}

// v の末尾に 要素 value を追加する。
// 計算量は amortized O(1)
// ex) v = {1, 3, 5, 7, 9}; の時、push_back(&v, 11); を呼ぶと、
//     v = {1, 3, 5, 7, 9, 11}; になる
void push_back(vector* v, vector_t value) {
    if(v->tail + 1 < v->capacity) {
        v->dat[++v->tail] = value;
    } else if(v->capacity == 0) {
        v->capacity = 1;
        v->tail = 0;
        v->dat = (vector_t*)calloc(1, sizeof(vector_t));
        v->dat[0] = value;
    } else {
        vector_t* new_dat = (vector_t*)calloc(v->capacity * 2, sizeof(vector_t));
        for(int i = 0; i <= v->tail; i++) {
            new_dat[i] = v->dat[i];
        }
        new_dat[++v->tail] = value;
        free(v->dat);
        v->dat = new_dat;
        v->capacity *= 2;
    }
}
