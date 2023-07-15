#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "trie.h"
#include "word.h"
#include "io.h"
#include "endian.h"
#include "code.h"

#define OPTIONS "i:o:hv"

int bit_len(uint16_t a) {
    int bit_count = 0;
    while (a > 0) {
        bit_count += 1;
        a >>= 1;
    }
    return bit_count;
}

int main(int argc, char **argv) {
    bool verbose = false;

    bool in = false;
    bool out = false;

    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;

    FileHeader header;

    struct stat infiles;
    struct stat outfiles;

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': verbose = true; break;

        case 'i':
            infile = open(optarg, O_RDONLY);
            fstat(infile, &infiles);
            header.magic = MAGIC;
            header.protection = infiles.st_mode;
            in = true;
            break;

        case 'o':
            outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC);
            fchmod(outfile, 0600);
            out = true;
            break;

        case 'h':
            printf("SYNOPSIS\n");
            printf("    Decompresses files using the LZ78 compression algorithm.\n");
            printf("    Used with files compressed with the corresponding encoder.\n");
            printf("\nUSAGE\n");
            printf("    ./decode [-vh] [-i input] [-o output]\n");
            printf("\nOPTIONS\n");
            printf("    -v          Display compression statistics\n");
            printf("    -i input    Specify input to decompress (stdin by default)\n");
            printf("    -o output   Specify output of decompressed input (stdout by default)\n");
            printf("    -h          Display program help and usage\n");
            break;
        }
    }

    if (in == false) {
        fstat(infile, &infiles);
        header.magic = MAGIC;
        header.protection = infiles.st_mode;
    }

    read_header(infile, &header);

    WordTable *table = wt_create();

    uint8_t curr_sym = 0;
    uint16_t curr_code = 0;

    uint16_t next_code = START_CODE;

    while (read_pair(infile, &curr_code, &curr_sym, bit_len(next_code)) == true) {
        table[next_code] = word_append_sym(table[curr_code], curr_sym);
        write_word(outfile, table[next_code]);
        next_code++;
        if (next_code == MAX_CODE) {
            wt_reset(table);
            next_code = START_CODE;
        }
    }

    flush_words(outfile);
    wt_delete(table);

    if (verbose == true) {
        fstat(infile, &infiles);
        fstat(outfile, &outfiles);
        size_t com = infiles.st_size + 1;
        size_t uncom = outfiles.st_size;

        printf("Compressed file size: %zu bytes\n", com);
        printf("Uncompressed file size: %zu bytes\n", uncom);
        double formula = 100.0 * (1.0 - (double) com / (double) uncom);
        printf("Space Saving: %.2f%%\n", formula);
    }

    if (in == true) {
        close(infile);
    }

    if (out == true) {
        close(outfile);
    }

    return 0;
}
