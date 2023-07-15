#include <stdio.h>
#include <unistd.h>
#include "endian.h"
#include "word.h"
#include "io.h"
#include "code.h"

static int syms = 0;
static uint8_t symbuf[BLOCK];

static int bits = 0;
static uint8_t bitbuf[BLOCK];

static int size = 0;

uint64_t total_syms = 0;
uint64_t total_bits = 0;

uint64_t bytes_read = 0;
uint64_t bytes_write = 0;

int read_bytes(int infile, uint8_t *buf, int to_read) {
    int i = 0;
    int j = 0;
    while ((i = read(infile, buf + j, to_read - j)) > 0) {
        j += i;
    }
    if (j == to_read) {
        bytes_read += to_read;
        return bytes_read;
    } else {
        bytes_read += j;
        return j;
    }
}

int write_bytes(int outfile, uint8_t *buf, int to_write) {
    int i = 0;
    int j = 0;
    while ((i = write(outfile, buf + j, to_write - j)) > 0) {
        j += i;
    }
    if (j == to_write) {
        bytes_write += to_write;
        return bytes_write;
    } else {
        bytes_write += j;
        return j;
    }
}

void read_header(int infile, FileHeader *header) {
    read_bytes(infile, (uint8_t *) header, sizeof(FileHeader));
    if (big_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
}

void write_header(int outfile, FileHeader *header) {
    write_bytes(outfile, (uint8_t *) header, sizeof(FileHeader));
    if (big_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
}

bool read_sym(int infile, uint8_t *sym) {
    if (!syms) {
        int bytes = read_bytes(infile, symbuf, BLOCK);
        if (bytes < BLOCK) {
            size = bytes + 1;
        }
    }
    *sym = symbuf[syms];
    syms++;
    syms = syms % BLOCK;
    if (syms != size) {
        total_syms += 1;
        return true;
    } else {
        return false;
    }
}

void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen) {
    int i;
    for (i = 0; i < bitlen; i++) {
        if (code & (1 << i)) {
            bitbuf[bits / 8] |= (1 << (bits % 8));
        } else {
            bitbuf[bits / 8] &= ~(1 << (bits % 8));
        }
        bits++;
        if (bits / 8 == BLOCK) {
            flush_pairs(outfile);
            bits = 0;
        }
        total_bits++;
    }
    for (i = 0; i < 8; i++) {
        if (sym & (1 << (i % 8))) {
            bitbuf[bits / 8] |= (1 << (bits % 8));
        } else {
            bitbuf[bits / 8] &= ~(1 << (bits % 8));
        }
        bits++;
        if (bits / 8 == BLOCK) {
            flush_pairs(outfile);
            bits = 0;
        }
        total_bits++;
    }
}

void flush_pairs(int outfile) {
    if (bits < (BLOCK * 8)) {
        write_bytes(outfile, bitbuf, bits / 8);
        for (int i = 0; i < BLOCK; i++) {
            bitbuf[i] = 0;
        }
    } else {
        write_bytes(outfile, bitbuf, BLOCK);
        for (int i = 0; i < BLOCK; i++) {
            bitbuf[i] = 0;
        }
    }
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    int i;
    *code = 0;
    *sym = 0;
    for (i = 0; i < bitlen; i++) {
        read_bytes(infile, bitbuf, BLOCK);
        if (bitbuf[bits / 8] & (1 << bits % 8)) {
            *code |= (1 << i);
        } else {
            *code &= ~(1 << i);
        }
        bits = (bits + 1) % (BLOCK * 8);
        total_bits++;
    }
    for (i = 0; i < 8; i++) {
        read_bytes(infile, bitbuf, BLOCK);
        if (bitbuf[bits / 8] & (1 << bits % 8)) {
            *sym |= (1 << i);
        } else {
            *sym &= ~(1 << i);
        }
        bits = (bits + 1) % (BLOCK * 8);
        total_bits++;
    }
    if (*code != STOP_CODE) {
        return true;
    } else {
        return false;
    }
}

void write_word(int outfile, Word *w) {
    for (uint8_t i = 0; i < w->len; i++) {
        symbuf[syms] = w->syms[i];
        syms++;
        if (syms == BLOCK) {
            write_bytes(outfile, symbuf, BLOCK);
            syms = 0;
            flush_words(outfile);
        }
    }
}

void flush_words(int outfile) {
    if (syms < BLOCK) {
        write_bytes(outfile, symbuf, syms);
        for (int i = 0; i < BLOCK; i++) {
            symbuf[i] = 0;
        }
    } else {
        write_bytes(outfile, symbuf, BLOCK);
        for (int i = 0; i < BLOCK; i++) {
            symbuf[i] = 0;
        }
    }
}
