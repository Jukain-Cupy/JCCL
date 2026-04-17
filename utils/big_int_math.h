#ifndef BIG_INT_MATH_H
#define BIG_INT_MATH_H

#include "big_int.h"

// Forward declaration (so compiler doesn't complain)
static inline BigInt* BI_add(BigInt* a, BigInt* b);

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
    if(!a || !b){
        return NULL;
    }

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
    if(!a || !b){
        return NULL;
    }

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

    return (n->head->digits % 2ULL) == 0;
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

// Computes n / 2
// @return Pointer to BigInt value representing n / 2
static inline BigInt* BI_divideBy2(BigInt* n){
    if(!n){
        return NULL;
    }

    if(n->sign == 0){
        return BI_create("0");
    }

    BigInt* result = BI_copy(n);

    if(!result){
        return NULL;
    }

    const unsigned long long BASE = BI_getLimit();
    unsigned long long carry = 0;

    Node* curr = result->tail;

    while(curr){
        unsigned long long val = carry * BASE + curr->digits;

        curr->digits = val / 2ULL;
        carry = val % 2ULL;

        curr = curr->prev;
    }

    return BI_normalise(result);
}

// Computes n * 2
// @return Pointer to BigInt value representing n * 2
static inline BigInt* BI_multiplyBy2(BigInt* n){
    if(!n){
        return NULL;
    }

    if(n->sign == 0){
        return BI_create("0");
    }

    BigInt* result = BI_copy(n);

    if(!result){
        return NULL;
    }

    const unsigned long long BASE = BI_getLimit();
    unsigned long long carry = 0;

    Node* curr = result->head;

    while(curr){
        unsigned long long val = curr->digits * 2ULL + carry;

        curr->digits = val % BASE;
        carry = val / BASE;
        
        curr = curr->next;
    }

    if(carry){
        Node* node = (Node*)calloc(1, sizeof(Node));
    
        if(!node){
            BI_free(result);

            return NULL;
        }

        node->digits = carry;
        node->prev = result->tail;

        result->tail->next = node;
        result->tail = node;
        result->size++;
    }

    return result;
}

// Computes a mod b
// @return Pointer to value representing a mod b
static inline BigInt* BI_mod(BigInt* a, BigInt* b){
    if(!a || !b){
        return NULL;
    }

    if(b->sign == 0){
        return NULL;
    }

    BigInt* rem = BI_copy(a);

    if(!rem){
        return NULL;
    }

    rem->sign = 1;

    BigInt* div = BI_copy(b);

    if(!div){
        BI_free(rem);

        return NULL;
    }

    div->sign = 1;

    if(BI_compareMagnitudes(rem, div) < 0){
        BI_free(div);

        return rem;
    }

    while(BI_compareMagnitudes(rem, div) >= 0){
        BigInt* tmp = BI_copy(div);
        BigInt* next = NULL;

        while(1){
            next = BI_multiplyBy2(tmp);

            if(BI_compareMagnitudes(next, rem) > 0){
                BI_free(next);

                break;
            }

            BI_free(tmp);

            tmp = next;
        }

        BigInt* newRem = BI_subtract(rem, tmp);

        BI_free(rem);
        BI_free(tmp);

        rem = newRem;
    }

    BI_free(div);

    return BI_normalise(rem);
}

// Computes base^exponent
// @param base Base for the exponentiation
// @param exp Exponent for the exponentiation
// @result Pointer too BigInt value representing base^exponent
static inline BigInt* BI_pow(BigInt* base, BigInt* exp){
    if(exp->sign < 0){
        return NULL;
    }

    BigInt* result = BI_create("1");
    BigInt* b = BI_copy(base);
    BigInt* e = BI_copy(exp);

    while(e->sign != 0){
        if(!BI_isEven(e)){
            BigInt* tmp = BI_multiply(result, b);    
            BI_free(result);
            result = tmp;
        }

        BigInt* bSquared = BI_multiply(b, b);
        BI_free(b);
        b = bSquared;

        BigInt* eHalved = BI_divideBy2(e);
        BI_free(e);
        e = eHalved;
    }

    BI_free(b);
    BI_free(e);

    return result;
}

// Computes base^exponent over a modulus mod
// @param base Base for the exponentiation
// @param exp Exponent for the exponentiation
// @param mod Modulus to compute exponentiation over
// @return Pointer to BigInt value representing base^exponent over a modulus mod
static inline BigInt* BI_powMod(BigInt* base, BigInt* exp, BigInt* mod){
    if(!base || !exp || !mod){
        return NULL;
    }

    if(exp->sign < 0){
        return NULL;
    }

    BigInt* result = BI_create("1");

    if(!result){
        return NULL;
    }

    BigInt* b = BI_mod(base, mod);
    
    if(!b){
        BI_free(result);

        return NULL;
    }

    BigInt* e = BI_copy(exp);
    
    if(!e){
        BI_free(result);
        BI_free(b);

        return NULL;
    }

    while(e->sign != 0){
        if(!BI_isEven(e)){
            BigInt* mul = BI_multiply(result, b);

            if(!mul){
                BI_free(result);
                BI_free(b);
                BI_free(e);

                return NULL;
            }

            BigInt* reduced = BI_mod(mul, mod);
            
            BI_free(mul);

            if(!reduced){
                BI_free(result);
                BI_free(b);
                BI_free(e);

                return NULL;
            }

            BI_free(result);

            result = reduced;
        }

        BigInt* bSquared = BI_multiply(b, b);
        
        if(!bSquared){
            BI_free(result);
            BI_free(b);
            BI_free(e);

            return NULL;
        }

        BigInt* bSqReduced = BI_mod(bSquared, mod);

        BI_free(bSquared);

        if(!bSqReduced){
            BI_free(result);
            BI_free(b);
            BI_free(e);

            return NULL;
        }

        BI_free(b);

        b = bSqReduced;

        BigInt* eHalved = BI_divideBy2(e);

        if(!eHalved){
            BI_free(result);
            BI_free(b);
            BI_free(e);

            return NULL;
        }

        BI_free(e);

        e = eHalved;
    }

    BI_free(b);
    BI_free(e);

    return result;
}

#endif