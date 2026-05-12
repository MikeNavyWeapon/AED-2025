#include <stdio.h>

int hash(int key) {
    return key % 10;
}
void Insert(int key) {
    int index = hash(key);
    printf("Inserting key %d at index %d\n", key, index);
}

int probe(int key) {
    int index = hash(key);
    printf("Probing for key %d at index %d\n", key, index);
    return index;
}
void Search(int key) {
    int index = probe(key);
    printf("Searching for key %d at index %d\n", key, index);
}

void main() {
    Insert(15);
    Insert(25);
    Insert(35);
}