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
    struct stat files;
    struct stat outfiles;

    bool verbose = false;

    bool in = false;
    bool out = false;

    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;

    FileHeader *header = calloc(1, sizeof(FileHeader));

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': verbose = true; break;

        case 'i':
            infile = open(optarg, O_RDONLY);
            in = true;
            break;

        case 'o':
            outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC);
            fchmod(outfile, 0600);
            fstat(outfile, &outfiles);
            header->magic = MAGIC;
            header->protection = outfiles.st_mode;
            out = true;
            break;

        case 'h':
            printf("SYNOPSIS\n");
            printf("    Compresses files using the LZ78 compression algorithm.\n");
            printf("    Compressed files are decompressed with the corresponding decoder.\n");
            printf("\nUSAGE\n");
            printf("    ./encode [-vh] [-i input] [-o output]\n");
            printf("\nOPTIONS\n");
            printf("    -v          Display compression statistics\n");
            printf("    -i input    Specify input to compress (stdin by default)\n");
            printf("    -o output   Specify output of compressed input (stdout by default)\n");
            printf("    -h          Display program help and usage\n");
            break;
        }
    }

    if (out == false) {
        fstat(outfile, &outfiles);
        header->magic = MAGIC;
        header->protection = outfiles.st_mode;
    }

    write_header(outfile, header);

    TrieNode *root = trie_create();
    TrieNode *curr_node = root;

    uint16_t next_code = START_CODE;

    TrieNode *prev_node = NULL;

    uint8_t prev_sym = 0;

    uint8_t curr_sym = 0;

    while (read_sym(infile, &curr_sym) == true) {
        TrieNode *next_node = trie_step(curr_node, curr_sym);
        if (next_node != NULL) {
            prev_node = curr_node;
            curr_node = next_node;
        } else {
            write_pair(outfile, curr_node->code, curr_sym, bit_len(next_code));
            curr_node->children[curr_sym] = trie_node_create(next_code);
            curr_node = root;
            next_code++;
        }
        if (next_code == MAX_CODE) {
            trie_reset(root);
            curr_node = root;
            next_code = START_CODE;
        }
        prev_sym = curr_sym;
    }

    if (curr_node != root) {
        write_pair(outfile, prev_node->code, prev_sym, bit_len(next_code));
        next_code = (next_code + 1) % MAX_CODE;
    }

    write_pair(outfile, STOP_CODE, 0, bit_len(next_code));
    flush_pairs(outfile);

    free(header);
    trie_delete(root);

    if (verbose == true) {
        fstat(infile, &files);
        fstat(outfile, &outfiles);
        size_t com = outfiles.st_size + 1;

        printf("Compressed file size: %zu bytes\n", com);
        printf("Uncompressed file size: %" PRIu64 " bytes\n", total_syms);
        double formula = 100.0 * (1.0 - (double) com / (double) total_syms);
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
