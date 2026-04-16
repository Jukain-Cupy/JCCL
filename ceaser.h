#ifndef CEASER_H
#define CEASER_H

#include <string.h>

// Encrypt a character by a certain shift
static inline char JCCL_ceaserEncryptChar(char c, int shift){
    char encryptedC;

    // Support uppercase
    if(c >= 'A' && c <= 'Z'){
        encryptedC = ((c - 'A' + shift) % 26) + 'A';
    
    // Support lowercase
    }else if(c >= 'a' && c <= 'z'){
        encryptedC = ((c - 'a' + shift) % 26) + 'a';
    
    // Keep non-letters the same
    }else{
        encryptedC = c;
    }

    return encryptedC;
}

// Decrypt a character that was encrypted by a certain shift
static inline char JCCL_ceaserDecryptChar(char c, int shift){
    int decryptShift = 26 - (shift % 26);

    return JCCL_ceaserEncryptChar(c, decryptShift);
}

// Encrypt an entire string by a certain shift, storing result in outStr
static inline char* JCCL_ceaserEncryptStr(char* str, int shift, char* outStr){
    for(int i=0; i<strlen(str); i++){
        outStr[i] = JCCL_ceaserEncryptChar(str[i], shift);
    }

    outStr[strlen(str)] = '\0';
}

// Decrypt an entire string that was encrypted by a certain shift, storing result in outStr
static inline void JCCL_ceaserDecryptStr(char* str, int shift, char* outStr){
    for(int i=0; i<strlen(str); i++){
        outStr[i] = JCCL_ceaserDecryptChar(str[i], shift);
    }

    outStr[strlen(str)] = '\0';
}

#endif