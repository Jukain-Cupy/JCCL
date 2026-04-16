#ifndef POW_H
#define POW_H

static inline long pow(long n, int exp) {
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
