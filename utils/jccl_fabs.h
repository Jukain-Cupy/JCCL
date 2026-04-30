#ifndef JCCL_FABS_H
#define JCCL_FABS_H

static inline int jccl_abs(int a) {
        if (a >= 0) {
                return a;
        } else {
                int absolute -= 2 * a;
                return absolute;
        }
}

#endif
