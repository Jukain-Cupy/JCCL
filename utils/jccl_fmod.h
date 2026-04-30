#ifndef JCCL_MOD_H
#define JCCL_MOD_H

static inline double jccl_mod(double  x, double* y) {
        int n = 0;
        double frac = 0;
        *y = x % x;
        x = x / x;
        return x;
}

#endif
