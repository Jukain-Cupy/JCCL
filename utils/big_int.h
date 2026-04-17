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

// Forward declaration (so compiler doesn't complain)
static inline BigInt* BI_add(BigInt* a, BigInt* b);

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

    bigNum->head = NULL;
    bigNum->tail = NULL;
    bigNum->size = 0;
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

// Compare magnitudes of two BigInt values (ignores sign)
// @return Value of 1 if |a| > |b|, -1 if |b| < |a| and 0 if |a| = |b|
static inline int BI_compareMagnitudes(BigInt* a, BigInt* b){
    // Trivial cases
    if(a->size > b->size){
        return 1;
    }

    if(a->size < b->size){
        return -1;
    }

    // Compare from most significant chunk
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

// Removes leading zero chunks (from most significant chunk)
// @param bigNum BigInt value to remove leading zeroes from
// @return Pointer to normalised BigInt value
static inline BigInt* BI_normalise(BigInt* bigNum){
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

    // If number is zero -> set sign = 0
    if(bigNum->size == 1 && bigNum->head->digits == 0){
        bigNum->sign = 0;
    }

    return bigNum;
}

// Computes |a| - |b|, given a >= b
// @return Pointer to BigInt value representing |a| - |b|
static inline BigInt* BI_subtractMagnitudes(BigInt* a, BigInt* b){
    if(BI_compareMagnitudes(a, b) < 0){
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
    const unsigned long long LIMIT = BI_getLimit();

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
            BI_free(result);

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

    return BI_normalise(result);
}

// Computes |a| + |b|
// @return Pointer to BigInt value representing |a| + |b|
static inline BigInt* BI_addMagnitudes(BigInt* a, BigInt* b){
    BigInt* result = (BigInt*)calloc(1, sizeof(BigInt));

    if(!result){
        return NULL;
    }

    result->sign = 1;

    Node* currA = a->head;
    Node* currB = b->head;

    unsigned long long carry = 0;
    // Sets a limiting value for moving to next chunk
    const unsigned long long LIMIT = BI_getLimit();

    // Traverses from least significant chunks
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

// Full subtraction with sign handling
// @return Pointer to BigInt value representing a - b
static inline BigInt* BI_subtract(BigInt* a, BigInt* b){    
    if(!a || !b){
        return NULL;
    }

    // Handles zero cases
    if(a->sign == 0){
        BigInt* result = BI_copy(b);

        if(result && result->sign != 0){
            result->sign = -result->sign;

            return result;
        }
    }

    if(b->sign == 0){
        return BI_copy(a);
    }

    BigInt* result = NULL;

    // Same sign -> subtraction
    if(a->sign == b->sign){
        // Result depends on the magnitudes
        int cmp = BI_compareMagnitudes(a, b);

        if(a->sign > 0){
            if(cmp >= 0){
                result = BI_subtractMagnitudes(a, b);

                if(result){
                    result->sign = 1;
                }
            
            }else{
                result = BI_subtractMagnitudes(b, a);

                if(result){
                    result->sign = -1;
                }
            }        
        }else{
            if(cmp >= 0){
                result = BI_subtractMagnitudes(a, b);

                if(result){
                    result->sign = -1;
                }
            
            }else{
                result = BI_subtractMagnitudes(b, a);

                if(result){
                    result->sign = 1;
                }
            } 
        }
    
    // If a is +ve and b is -ve
    }else if(a->sign > 0 && b->sign < 0){
        result = BI_addMagnitudes(a, b);

        if(result){
            result->sign = 1;
        }
    
    // If a is -ve and b is +ve
    }else{
        result = BI_addMagnitudes(a, b);

        if(result){
            result->sign = -1;
        }
    }

    if(result){
        BI_normalise(result);

        // Set sign for 0 value
        if(result->size == 1 && result->head->digits == 0){
            result->sign = 0;
        }
    }

    return result;
}

// Full addition with sign handling
// @return Pointer to BigInt value representing a + b
static inline BigInt* BI_add(BigInt* a, BigInt* b){    
    if(!a || !b){
        return NULL;
    }

    // Zero cases
    if(a->sign == 0){
        return BI_copy(b);
    }

    if(b->sign == 0){
        return BI_copy(a);
    }

    BigInt* result = NULL;

    // Same sign -> magnitude addition
    if(a->sign == b->sign){
        result = BI_addMagnitudes(a, b);
        
        if(result){
            result->sign = a->sign;
        }

    // If a is +ve and b is -ve
    }else if(a->sign > 0 && b->sign < 0){
        int cmp = BI_compareMagnitudes(a, b);
        
        if(cmp >= 0){
            result = BI_subtractMagnitudes(a, b);
            if(result){ 
                result->sign = 1;
            }
        
        }else{
            result = BI_subtractMagnitudes(b, a);
            
            if(result){
                result->sign = -1;
            }
        }

    // If a is -ve and b is +ve
    }else{
        int cmp = BI_compareMagnitudes(a, b);
        if(cmp >= 0){
            result = BI_subtractMagnitudes(a, b);
            if(result){
                result->sign = -1;
            }
        
        }else{
            result = BI_subtractMagnitudes(b, a);
            
            if(result){
                result->sign = 1;
            }
        }
    }

    if(result){
        BI_normalise(result);

        // Set sign for 0 value
        if(result->size == 1 && result->head->digits == 0){
            result->sign = 0;
        }
    }

    return result;
}

// Full multiplication with sign handling
// @return Pointer to BigInt value representing a * b
static inline BigInt* BI_multiply(BigInt* a, BigInt* b){
    if(!a || !b){
        return NULL;
    }

    // Handles zero cases
    if(a->sign == 0 || b->sign == 0){
        return BI_create("0");
    }

    const unsigned long long LIMIT = BI_getLimit();

    BigInt* result = (BigInt*)calloc(1, sizeof(BigInt));

    if(!result){
        return NULL;
    }

    result->sign = a->sign * b->sign;

    // Allocate result nodes (max size = a->size + b->size)
    for(int i=0; i<a->size + b->size; i++){
        Node* node = (Node*)calloc(1, sizeof(Node));

        if(!node){
            BI_free(result);
        
            return NULL;
        }

        // Append node to result list
        if(!result->head){
            result->head = result->tail = node;
        
        }else{
            node->prev = result->tail;
            result->tail->next = node;
            result->tail = node;
        }

        result->size++;
    }

    Node* currA = a->head;
    Node* resultRow = result->head;

    for(int i=0; i<a->size; i++){
        Node* currB = b->head;
        Node* resultCol = resultRow;

        unsigned long long carry = 0;

        for(int j=0; j<b->size; j++){
            unsigned long long mul = currA->digits * currB->digits + resultCol->digits + carry;

            resultCol->digits = mul % LIMIT;
            carry = mul / LIMIT;

            currB = currB->next;
            resultCol = resultCol->next;
        }

        if(carry){
            resultCol->digits += carry;
        }

        currA = currA->next;
        resultRow = resultRow->next;
    }

    BI_normalise(result);

    return result;
}

// Checks if BigInt value represents zero
// @return Value of 1 if n = 0, 0 if not
static inline int BI_isZero(BigInt* n){
    return n && n->sign == 0;
}

// Checks if BigInt value represents an even number
// @return Value of 1 if n is even, 0 if not
static inline int BI_isEven(BigInt* n){
    if(!n || !n->head){
        return 0;
    }

    return (n->head->digits % 2ULL) != 0;
}

// Compare two BigInt values
// @return Value of 1 if a > b, -1 if b < a and 0 if a = b
static inline int BI_compare(BigInt* a, BigInt* b){
    if(a->sign > b->sign){
        return 1;
    }

    if(a->sign < b->sign){
        return -1;
    }

    if(a->sign == 0){
        return 0;
    }

    int cmp = BI_compareMagnitudes(a, b);

    if(a->sign > 0){
        return cmp;
    
    }else{
        return -cmp;
    }
}

#endif