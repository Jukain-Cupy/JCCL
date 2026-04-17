#ifndef BIG_INT_H
#define BIG_INT_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Number of digits per node
#define CHUNK_SIZE 9

// Doubly linked list node storing a chunk of digits
typedef struct Node{
    unsigned long long digits;
    struct Node* next;
    struct Node* prev;
} Node;

// BigInt representation
typedef struct BigInt{
    Node* head; // Least significant chunk
    Node* tail; // Most significant chunk
    int size;   // Number of chunks
    int sign;   // -1 (-ve), 0 (zero), 1 (+ve)
} BigInt;

// Frees all nodes and the BigInt itself
// @param bigNum Pointer to BigInt value to free
static inline void BI_free(BigInt* bigNum){
    if(!bigNum){
        return;
    }

    // Traverse bigNum
    Node* current = bigNum->head;
    while(current){
        Node* tmp = current;
        current = current->next;

        // Free nodes as you pass through
        free(tmp);
    }

    free(bigNum);
}

// Parses a string into a BigInt
// @param numStr The number the BigInt will represent
// @return Pointer to BigInt value
static inline BigInt* BI_create(const char* numStr){
    if(numStr == NULL || numStr[0] == '\0'){
        return NULL;
    }

    BigInt* bigNum = (BigInt*)calloc(1, sizeof(BigInt));

    if(!bigNum){
        return NULL;
    }

    bigNum->sign = 1;

    int startIndex = 0;

    // Handles negative numbers
    if(numStr[0] == '-'){
        bigNum->sign = -1;
        startIndex = 1;
    }

    int strLen = strlen(numStr);

    // Skips leading zeroes
    while(startIndex < strLen && numStr[startIndex] == '0'){
        startIndex++;
    }

    // If all zeroes -> allocate single zero node
    if(startIndex == strLen){
        bigNum->sign = 0;
        Node* zeroNode = (Node*)calloc(1, sizeof(Node));

        if(!zeroNode){
            free(bigNum);

            return NULL;
        }

        bigNum->head = bigNum->tail = zeroNode;
        bigNum->size = 1;

        return bigNum;
    }

    // Iterate over the string, starting at the end
    for(int i=strLen-1; i>=startIndex; i-=CHUNK_SIZE){
        Node* newNode = (Node*)calloc(1, sizeof(Node));

        if(!newNode){
            BI_free(bigNum);

            return NULL;
        }

        newNode->prev = bigNum->tail;

        unsigned long long chunk = 0;
        unsigned long long multiplier = 1;

        // Build chunk one digit at a time
        for(int j=0; j<CHUNK_SIZE && (i-j) >= startIndex; j++){
            char digit = numStr[i-j];

            if(digit < '0' || digit > '9'){
                free(newNode);
                BI_free(bigNum);

                return NULL;
            }

            // Update the current chunk
            chunk += (unsigned long long)(digit - '0') * multiplier;
            multiplier *= 10;
        }

        newNode->digits = chunk;

        // Append to list (with least significant chunk at head)
        if(bigNum->tail == NULL){
            bigNum->head = newNode;
            bigNum->tail = newNode;
        }else{
            bigNum->tail->next = newNode;
            bigNum->tail = newNode;
        }

        bigNum->size++;
    }

    return bigNum;
}

// Deep copy of BigInt
// @param src Pointer to the original BigInt value that needs copying
// @return Pointer to copied BigInt value
static inline BigInt* BI_copy(const BigInt* src){
    if(!src){
        return NULL;
    }

    BigInt* dest = (BigInt*)calloc(1, sizeof(BigInt));

    if(!dest){
        return NULL;
    }

    dest->sign = src->sign;
    dest->size = 0;
    dest->head = NULL;
    dest->tail = NULL;

    if(src->size == 0){
        return dest;
    }

    Node* currSrc = src->head;
    Node* prevDest = NULL;

    while(currSrc){
        Node* newNode = (Node*)calloc(1, sizeof(Node));

        if(!newNode){
            BI_free(dest);

            return NULL;
        }

        // Copies over the same digits from src
        newNode->digits = currSrc->digits;
        newNode->prev = prevDest;
        newNode->next = NULL;

        if(!dest->head){
            dest->head = newNode;
        
        }else{
            prevDest->next = newNode;
        }

        dest->tail = newNode;
        dest->size++;

        prevDest = newNode;
        currSrc = currSrc->next;
    }

    return dest;
}

// Prints digits of a node recursively (with most significant chunk first)
// @param node The node to recurse over
static inline void BI_printNodeRecursive(Node* node){
    if(node == NULL){
        return;
    }

    BI_printNodeRecursive(node->next);

    // Prints most significant chunk normally
    if(node->next == NULL){
        printf("%llu", node->digits);
    
    }else{
        // Pads with leading zeroes
        printf("%0*llu", CHUNK_SIZE, node->digits);
    }
}

// Prints BigInt
// @param bigNum Pointer to BigInt value to print
static inline void BI_print(BigInt* bigNum){
    if(!bigNum || bigNum->head == NULL){
        return;
    }

    if(bigNum->sign == 0){
        printf("0");
        
        return;
    }

    if(bigNum->sign < 0){
        printf("-");
    }

    BI_printNodeRecursive(bigNum->head);
}

// Computes 10^CHUNK_SIZE once and caches it
static inline unsigned long long BI_getLimit() {
    static unsigned long long limit = 0;

    if (limit == 0) {
        limit = 1;

        for(int i=0; i<CHUNK_SIZE; i++){
            limit *= 10;
        }
    }

    return limit;
}

#endif