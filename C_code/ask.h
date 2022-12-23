#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vector.h"
#include "myrandom.h"
#include "constraint.h"

char* ask(int query_id, const char* answer_filename) {
    fprintf(stderr, "ask from query_id = %d\n", query_id);

    FILE* fans = fopen(answer_filename, "r");
    int p_ins, p_sub, p_del, ansId;
    fscanf(fans, "%d %d %d", &p_ins, &p_sub, &p_del);
    char base[100 + 1];
    for(int i = 0; i < query_id; i++) {
        fscanf(fans, "%d %s", &ansId, base);
    }

    vector qs;

    while(1) {
        qs = makeVector(0);
        int len = strlen(base);
        for(int i = 0; i < len; i++) {
            push_back(&qs, base[i]);
        }

        // add deletion error
        vector tmp = makeVector(0);
        for(int j = 0; j < getVectorSize(&qs); j++) {
            double prob = (double)myrand() / __UINT32_MAX__;
            if(prob <= (double)p_del / 100) {
                continue;
            }
            push_back(&tmp, qs.dat[j]);
        }
        qs = tmp;
        // add substitution error
        for(int j = 0; j < getVectorSize(&qs); j++) {
            double prob = (double)myrand() / __UINT32_MAX__;
            if(prob <= (double)p_sub / 100) {
                while(1) {
                    char ch = (char)('0' + randint(0, SIGMA));
                    if(ch != qs.dat[j]) {
                        qs.dat[j] = ch;
                        break;
                    }
                }
            }
        }
        // add insertion error
        tmp = makeVector(0);
        for(int j = 0; j < getVectorSize(&qs); j++) {
            while(1) {
                double prob = (double)myrand() / __UINT32_MAX__;
                if(prob <= (double)p_ins / 100) {
                    push_back(&tmp, (char)('0' + randint(0, SIGMA)));
                } else {
                    break;
                }
            }
            push_back(&tmp, qs.dat[j]);
        }
        qs = tmp;
        int qslen = getVectorSize(&qs);
        if(QUERY_MIN_LENGTH <= qslen && qslen <= QUERY_MAX_LENGTH) {
            break;
        }
    }
    return qs.dat;
}