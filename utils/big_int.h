#ifndef BIG_INT_H
#define BIG_INT_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 18

typedef struct Node{
    unsigned long long digits;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct BigInt{
    Node* head;
    Node* tail;
    int size;
    int sign;
} BigInt;

// So the compiler doesn't complain
static inline BigInt* addBigInts(BigInt* a, BigInt* b);

// Frees the memory held by a BigInt pointer
// @param bigNum Pointer to BigInt value to free
static inline void freeBigInt(BigInt* bigNum){
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

// Creates a BigInt from a number in string form
// @param numStr The number the BigInt will represent
// @return Pointer to BigInt value
static inline BigInt* createBigInt(const char* numStr){
    if(numStr == NULL || numStr[0] == '\0'){
        return NULL;
    }

    BigInt* bigNum = (BigInt*)calloc(1, sizeof(BigInt));

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

    // Don't read leading zeroes
    while(startIndex < strLen && numStr[startIndex] == '0'){
        startIndex++;
    }

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

    Node* current = NULL;
    // Iterate over the string, starting at the end
    for(int i=strLen-1; i>=startIndex; i-=CHUNK_SIZE){
        Node* newNode = (Node*)calloc(1, sizeof(Node));

        if(!newNode){
            freeBigInt(bigNum);

            return NULL;
        }

        newNode->prev = bigNum->tail;

        unsigned long long chunk = 0;
        unsigned long long multiplier = 1;

        for(int j=0; j<CHUNK_SIZE && (i-j) >= startIndex; j++){
            char digit = numStr[i-j];

            if(digit < '0' || digit > '9'){
                free(newNode);
                freeBigInt(bigNum);

                return NULL;
            }

            // Update the current chunk
            chunk += (unsigned long long)(digit - '0') * multiplier;
            multiplier *= 10;
        }

        newNode->digits = chunk;

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

// Creates a copy for a BigInt value
// @param src Pointer to the original BigInt value that needs copying
// @return Pointer to copied BigInt value
static inline BigInt* copyBigInt(const BigInt* src){
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
            freeBigInt(dest);

            return NULL;
        }

        // Copy over the same digits from src
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

// Prints digits of a node recursively
// @param node The node to recurse over
static inline void printNodeRecursive(Node* node){
    if(node == NULL){
        return;
    }

    printNodeRecursive(node->next);

    if(node->next == NULL){
        printf("%llu", node->digits);
    
    }else{
        printf("%0*llu", CHUNK_SIZE, node->digits);
    }
}

// Prints the value represented by bigNum
// @param bigNum Pointer to BigInt value to print
static inline void printBigInt(BigInt* bigNum){
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

    printNodeRecursive(bigNum->head);
}

// Compare magnitudes of two BigInt values
// @return Value of 1 if |a| > |b|, -1 if |b| < |a| and 0 if |a| = |b|
static inline int compareMagnitudes(BigInt* a, BigInt* b){
    // Trivial cases
    if(a->size > b->size){
        return 1;
    }

    if(a->size < b->size){
        return -1;
    }

    // Iterate backwards
    Node* currA = a->tail;
    Node* currB = b->tail;

    while(currA != NULL && currB != NULL){
        if(currA->digits > currB->digits){
            return 1;
        }

        if(currA->digits < currB->digits){
            return -1;
        }

        currA = currA->prev;
        currB = currB->prev;
    }

    return 0;
}

static inline unsigned long long getLimit() {
    static unsigned long long limit = 0;

    if (limit == 0) {
        limit = 1;

        for(int i=0; i<CHUNK_SIZE; i++){
            limit *= 10;
        }
    }

    return limit;
}

// Removes leading zeroes in bigNum
// @param bigNum BigInt value to remove leading zeroes from
// @return Pointer to normalised BigInt value
static inline BigInt* normalise(BigInt* bigNum){
    if(!bigNum){
        return NULL;
    }

    while(bigNum->tail && bigNum->tail->digits == 0 && bigNum->size > 1){
        Node* tmp = bigNum->tail;
        
        bigNum->tail = bigNum->tail->prev;
        bigNum->tail->next = NULL;
        
        free(tmp);

        bigNum->size--;
    }

    if(bigNum->size == 1 && bigNum->head->digits == 0){
        bigNum->sign = 0;
    }

    return bigNum;
}

// Computes |a| - |b|, given a >= b
// @return Pointer to BigInt value representing |a| - |b|
static inline BigInt* subtractBigIntMagnitudes(BigInt* a, BigInt* b){
    if(compareMagnitudes(a, b) < 0){
        return NULL;
    }

    BigInt* result = (BigInt*)calloc(1, sizeof(BigInt));

    if(!result){
        return NULL;
    }

    // Iterate forwards
    Node* currA = a->head;
    Node* currB = b->head;

    unsigned long long borrow = 0;
    // Set a limiting value for moving to next chunk
    const unsigned long long LIMIT = getLimit();

    while(currA){
        unsigned long long valA = (unsigned long long)currA->digits;
        unsigned long long valB = currB ? (unsigned long long)currB->digits : 0;

        long long diff = valA - valB - borrow;

        if(diff < 0){
            diff += LIMIT;
            borrow = 1;
        
        }else{
            borrow = 0;
        }

        Node* newNode = (Node*)calloc(1, sizeof(Node));

        if(!newNode){
            freeBigInt(result);

            return NULL;
        }

        newNode->digits = (unsigned long long)diff;

        if(!result->head){
            result->head = result->tail = newNode;
        
        }else{
            newNode->prev = result->tail;
            
            result->tail->next = newNode;
            result->tail = newNode;
        }

        result->size++;

        currA = currA->next;

        if(currB){
            currB = currB->next;
        }
    }

    return normalise(result);
}

// Computes |a| + |b|
// @return Pointer to BigInt value representing |a| + |b|
static inline BigInt* addBigIntMagnitudes(BigInt* a, BigInt* b){
    BigInt* result = (BigInt*)calloc(1, sizeof(BigInt));

    if(!result){
        return NULL;
    }

    result->sign = 1;

    // Iterate forwards
    Node* currA = a->head;
    Node* currB = b->head;

    unsigned long long carry = 0;
    // Set a limiting value for moving to next chunk
    const unsigned long long LIMIT = getLimit();

    while(currA || currB || carry){
        unsigned long long valA = currA ? currA->digits : 0;
        unsigned long long valB = currB ? currB->digits : 0;
        unsigned long long sum = valA + valB + carry;

        carry = sum / LIMIT;

        Node* newNode = (Node*)calloc(1, sizeof(Node));

        if(!newNode){
            free(result);

            return NULL;
        }

        newNode->digits = sum % LIMIT;

        if(!result->head){
            result->head = result->tail = newNode;
        
        }else{
            newNode->prev = result->tail;

            result->tail->next = newNode;
            result->tail = newNode;
        }

        result->size++;

        if(currA){
            currA = currA->next;
        }

        if(currB){
            currB = currB->next;
        }
    }

    return result;
}

// Computes a - b
// @return Pointer to BigInt value representing a - b
static inline BigInt* subtractBigInts(BigInt* a, BigInt* b){    
    if(!a || !b){
        return NULL;
    }

    // Zero cases
    if(a->sign == 0){
        BigInt* result = copyBigInt(b);

        if(result && result->sign != 0){
            result->sign = -result->sign;

            return result;
        }
    }

    if(b->sign == 0){
        return copyBigInt(a);
    }

    BigInt* result = NULL;

    // Equal signs
    if(a->sign == b->sign){
        // Result depends on the magnitudes
        int cmp = compareMagnitudes(a, b);

        if(a->sign > 0){
            if(cmp >= 0){
                result = subtractBigIntMagnitudes(a, b);

                if(result){
                    result->sign = 1;
                }
            
            }else{
                result = subtractBigIntMagnitudes(b, a);

                if(result){
                    result->sign = -1;
                }
            }        
        }else{
            if(cmp >= 0){
                result = subtractBigIntMagnitudes(a, b);

                if(result){
                    result->sign = -1;
                }
            
            }else{
                result = subtractBigIntMagnitudes(b, a);

                if(result){
                    result->sign = 1;
                }
            } 
        }
    
    // If a is +ve and b is -ve
    }else if(a->sign > 0 && b->sign < 0){
        result = addBigIntMagnitudes(a, b);

        if(result){
            result->sign = 1;
        }
    
    // If a is -ve and b is +ve
    }else{
        result = addBigIntMagnitudes(a, b);

        if(result){
            result->sign = -1;
        }
    }

    if(result){
        normalise(result);

        // Set sign for 0 value
        if(result->size == 1 && result->head->digits == 0){
            result->sign = 0;
        }
    }

    return result;
}

// Computes a + b
// @return Pointer to BigInt value representing a + b
static inline BigInt* addBigInts(BigInt* a, BigInt* b){    
    if(!a || !b){
        return NULL;
    }

    // Zero cases
    if(a->sign == 0){
        return copyBigInt(b);
    }

    if(b->sign == 0){
        return copyBigInt(a);
    }

    BigInt* result = NULL;

    // Equal signs
    if(a->sign == b->sign){
        result = addBigIntMagnitudes(a, b);
        
        if(result){
            result->sign = a->sign;
        }

    // If a is +ve and b is -ve
    }else if(a->sign > 0 && b->sign < 0){
        int cmp = compareMagnitudes(a, b);
        
        if(cmp >= 0){
            result = subtractBigIntMagnitudes(a, b);
            if(result){ 
                result->sign = 1;
            }
        
        }else{
            result = subtractBigIntMagnitudes(b, a);
            
            if(result){
                result->sign = -1;
            }
        }

    // If a is -ve and b is +ve
    }else{
        int cmp = compareMagnitudes(a, b);
        if(cmp >= 0){
            result = subtractBigIntMagnitudes(a, b);
            if(result){
                result->sign = -1;
            }
        
        }else{
            result = subtractBigIntMagnitudes(b, a);
            
            if(result){
                result->sign = 1;
            }
        }
    }

    if(result){
        normalise(result);

        // Set sign for 0 value
        if(result->size == 1 && result->head->digits == 0){
            result->sign = 0;
        }
    }

    return result;
}

#endif