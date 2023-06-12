#include "../../inc/uchat.h"

// Переведення ciphertext з формату base64 та подальше його декодування ciphertext_len має бути той, який видала функція шифрування!
char *decrypt_data_stream(const char *ciphertext, int ciphertext_len) {
    int from_length = mx_strlen(ciphertext);
    char *from_base64 = mx_strnew(from_length);
    decode_base64((unsigned char*)ciphertext, from_length, (unsigned char*)from_base64);
    //printf("decrypt: |%s|\n", from_base64);
    
    int decryptedtext_len = ciphertext_len;
    char *decryptedtext = malloc(decryptedtext_len);
    
    EVP_CIPHER_CTX *ctx;

    if(!(ctx = EVP_CIPHER_CTX_new())) {
        return NULL;
    }

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, KEY, IV)) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    int len;
    int plaintext_written;

    if(1 != EVP_DecryptUpdate(ctx, (unsigned char *)decryptedtext, &decryptedtext_len, (unsigned char *)from_base64, ciphertext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    plaintext_written = decryptedtext_len;

    if(1 != EVP_DecryptFinal_ex(ctx, (unsigned char *)(decryptedtext + plaintext_written), &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    plaintext_written += len;

    EVP_CIPHER_CTX_free(ctx);

    decryptedtext_len = plaintext_written;
    free(from_base64);
    return decryptedtext;
}

// Декодування input повідомлення з формату base64
int decode_base64(const unsigned char *input, int input_len, unsigned char *output) {
  return EVP_DecodeBlock(output, input, input_len);
}
