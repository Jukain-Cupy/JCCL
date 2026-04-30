#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "big_int.h"
#include "jccl_rand.h"

#ifndef JCCL_PRIME_H
#define JCCL_PRIME_H

static inline int jccl_prime(int digits) {
        // how do I implement random number generation?
        BigInt *n = malloc(digts * sizeof(BigInt));
        // how do I randomly generate numbers?? Idk what kind of algorithm this is
        while (n->head) {
                n->digits = jccl_rand(jccl_rand(0));
                n = n->next;
        }
}

#endif
