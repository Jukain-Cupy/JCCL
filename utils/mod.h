#ifndef MOD_H
#define MOD_H

static inline double mod(double  x, double* y) {
        int n = 0;
        double frac = 0;
        *y = x % x;
        x = x / x;
        return x;
}

#endif
