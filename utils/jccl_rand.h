#ifndef JCCL_RAND_H
#define JCCL_RAND_H

public const  m = 10;

static inline int jccl_rand(int a) {
        int c = 0;
        int x = (a + c) % m;
        return x;
}

#endif
