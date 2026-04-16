#ifndef BIG_INT_H
#define BIG_INT_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 18

typedef struct Node{
    unsigned long long digits;
    struct Node* next;
} Node;

typedef struct BigInt{
    Node* head;
    Node* tail;
    int size;
    int sign;
} BigInt;

static inline void freeBigInt(BigInt* bigNum){
    if(!bigNum){
        return;
    }

    Node* current = bigNum->head;
    while(current){
        Node* tmp = current;
        current = current->next;

        free(tmp);
    }

    free(bigNum);
}

static inline BigInt* createBigInt(const char* numStr){
    if(numStr == NULL || numStr[0] == '\0'){
        return NULL;
    }

    BigInt* bigNum = (BigInt*)malloc(sizeof(BigInt));

    if(!bigNum){
        return NULL;
    }

    bigNum->head = NULL;
    bigNum->tail = NULL;
    bigNum->size = 0;
    bigNum->sign = 1;

    int startIndex = 0;
    if(numStr[0] == '-'){
        bigNum->sign = -1;
        startIndex = 1;
    }

    int strLen = strlen(numStr);

    while(startIndex < strLen && numStr[startIndex] == '0'){
        startIndex++;
    }

    if(startIndex == strLen){
        bigNum->sign = 0;
        Node* zeroNode = (Node*)malloc(sizeof(Node));

        if(!zeroNode){
            free(bigNum);

            return NULL;
        }

        zeroNode->digits = 0;
        zeroNode->next = NULL;

        bigNum->head = bigNum->tail = zeroNode;
        bigNum->size = 1;

        return bigNum;
    }

    Node* current = NULL;
    for(int i=strLen-1; i>=startIndex; i-=CHUNK_SIZE){
        Node* newNode = (Node*)malloc(sizeof(Node));

        if(!newNode){
            freeBigInt(bigNum);

            return NULL;
        }

        newNode->next = NULL;

        unsigned long long chunk = 0;
        unsigned long long multiplier = 1;

        for(int j=0; j<CHUNK_SIZE && (i-j) >= startIndex; j++){
            char digit = numStr[i-j];

            if(digit < '0' || digit > '9'){
                free(newNode);
                freeBigInt(bigNum);

                return NULL;
            }

            chunk += (digit - '0') * multiplier;
            multiplier *= 10;
        }

        newNode->digits = chunk;

        newNode->next = bigNum->head;
        bigNum->head = newNode;

        if(bigNum->tail == NULL){
            bigNum->tail = newNode;
        }

        bigNum->size++;
    }

    return bigNum;
}

static inline void printBigInt(BigInt* bigNum){
    if(!bigNum){
        return;
    }

    if(bigNum->sign < 0){
        printf("-");
    }

    Node* current = bigNum->head;

    if(current){
        printf("%llu", current->digits);
        current = current->next;
    }

    while(current){
        printf("%0*llu", CHUNK_SIZE, current->digits);
        current = current->next;
    }
}

#endif