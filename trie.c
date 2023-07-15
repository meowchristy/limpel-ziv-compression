#include <stdio.h>
#include <stdlib.h>
#include "trie.h"
#include "code.h"

TrieNode *trie_node_create(uint16_t code) {
    TrieNode *node = (TrieNode *) calloc(1, sizeof(TrieNode));
    node->code = code;
    return node;
}

void trie_node_delete(TrieNode *n) {
    free(n);
}

TrieNode *trie_create(void) {
    return trie_node_create(EMPTY_CODE);
}

void trie_reset(TrieNode *root) {
    for (uint16_t i = 0; i < ALPHABET; i++) {
        trie_delete(root->children[i]);
        root->children[i] = NULL;
    }
}

void trie_delete(TrieNode *n) {
    if (n) {
        for (uint16_t i = 0; i < ALPHABET; i++) {
            trie_delete(n->children[i]);
        }
        trie_node_delete(n);
        n = NULL;
    }
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
    return n->children[sym];
}
