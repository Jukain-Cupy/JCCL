#ifndef LOG_H
#define LOG_H

static inline int log (int  base, long  n) {
        int exp = 0;
        if (n == 0 || (n < base && n != 1)) {
                return -1; 
        } else if (n == 1) {
                exp =  0;
        } else {
                while (n > base) {
                        n /= base;
                        --exp;
                }
        }
        return exp;
}
#endif
