#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "word.h"
#include "code.h"

Word *word_create(uint8_t *syms, uint32_t len) {
    Word *w = (Word *) calloc(1, sizeof(Word));
    w->len = len;
    w->syms = (uint8_t *) calloc(len, sizeof(uint8_t));
    memcpy(w->syms, syms, len);
    return w;
}

Word *word_append_sym(Word *w, uint8_t sym) {
    Word *word = (Word *) calloc(1, sizeof(Word));
    word->len = w->len + 1;
    word->syms = (uint8_t *) calloc(word->len, sizeof(uint8_t));
    memcpy(word->syms, w->syms, w->len);
    word->syms[(word->len) - 1] = sym;
    return word;
}

void word_delete(Word *w) {
    free(w->syms);
    w->syms = NULL;
    free(w);
}

WordTable *wt_create(void) {
    WordTable *wt = (WordTable *) calloc(MAX_CODE, sizeof(Word *));
    wt[EMPTY_CODE] = (Word *) calloc(1, sizeof(Word));
    return wt;
}

void wt_reset(WordTable *wt) {
    for (uint16_t i = START_CODE; i < MAX_CODE; i++) {
        if (wt[i]) {
            word_delete(wt[i]);
            wt[i] = NULL;
        }
    }
}

void wt_delete(WordTable *wt) {
    for (uint16_t i = 0; i < MAX_CODE; i++) {
        if (wt[i]) {
            word_delete(wt[i]);
            wt[i] = NULL;
        }
    }
    free(wt);
}
