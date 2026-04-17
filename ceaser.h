#ifndef CEASER_H
#define CEASER_H

#include <string.h>

// Encrypts a character by a certain shift
// @param c Character to be encrypted
// @param shift Amount to shift by, negative values suggest shifting backwards
static inline char JCCL_ceaserEncryptChar(char c, int shift){
    char encryptedC;

    // Supports uppercase
    if(c >= 'A' && c <= 'Z'){
        encryptedC = ((c - 'A' + shift) % 26) + 'A';
    
    // Supports lowercase
    }else if(c >= 'a' && c <= 'z'){
        encryptedC = ((c - 'a' + shift) % 26) + 'a';
    
    // Keeps non-letters the same
    }else{
        encryptedC = c;
    }

    return encryptedC;
}

// Decrypts a character that was encrypted by a certain shift
// @param c Character to be decrypted
// @param shift Amount the character was originally shifted by
static inline char JCCL_ceaserDecryptChar(char c, int shift){
    int decryptShift = 26 - (shift % 26);

    return JCCL_ceaserEncryptChar(c, decryptShift);
}

// Encrypts an entire string by a certain shift, storing result in outStr
// @param str String to encrypt
// @param shift Amount to shift by, negative values suggest shfiting backwards
// @param outStr Pointer to where the output is stored
static inline void JCCL_ceaserEncryptStr(char* str, int shift, char* outStr){
    for(int i=0; i<strlen(str); i++){
        outStr[i] = JCCL_ceaserEncryptChar(str[i], shift);
    }

    outStr[strlen(str)] = '\0';
}

// Decrypts an entire string that was encrypted by a certain shift, storing result in outStr
// @param str String to decrypt
// @param shift Amount the string was originally shifted by
// @param outStr Pointer to where the output is stored
static inline void JCCL_ceaserDecryptStr(char* str, int shift, char* outStr){
    for(int i=0; i<strlen(str); i++){
        outStr[i] = JCCL_ceaserDecryptChar(str[i], shift);
    }

    outStr[strlen(str)] = '\0';
}

#endif