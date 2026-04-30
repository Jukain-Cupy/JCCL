#ifndef JCCL_POW_H
#define JCCL_POW_H

static inline long jccl_pow(long n, int exp) {
        int prod = n;
        if (exp == 0) {
                prod = 1;
        }
        for (int i = 0; i < exp; ++i) {
                prod *= n;
        }

        return prod;
}

#endif
