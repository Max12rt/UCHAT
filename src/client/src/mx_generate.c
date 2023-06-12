#include "../../inc/main.h"

// Кодування plaintext та переведення його у формат base64
char *encrypt_data_stream(const char *plaintext, int plaintext_len, int *length) {
    int ciphertext_len = plaintext_len + EVP_CIPHER_block_size(EVP_aes_256_cbc());
    char *ciphertext = malloc(ciphertext_len);
    
    EVP_CIPHER_CTX *ctx;

    if(!(ctx = EVP_CIPHER_CTX_new())) {
        return NULL;
    }

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, KEY, IV)) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    int len;
    int ciphertext_written;

    if(1 != EVP_EncryptUpdate(ctx, (unsigned char *)ciphertext, &ciphertext_len, (unsigned char *)plaintext, plaintext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    ciphertext_written = ciphertext_len;

    if(1 != EVP_EncryptFinal_ex(ctx, (unsigned char *)(ciphertext + ciphertext_written), &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    ciphertext_written += len;

    EVP_CIPHER_CTX_free(ctx);

    ciphertext_len = ciphertext_written;
    *length = ciphertext_len;
    
    // конвертація шифрованого повідомлення у base64 формат
    int hash_length = ciphertext_len * 2;
    char *hash_str = mx_strnew(hash_length);
    encode_base64((unsigned char*)ciphertext, ciphertext_len, hash_str, &hash_length);
    *length = ciphertext_len;
    //printf("encrypt: |%s|\n", ciphertext);
    free(ciphertext);
    return hash_str;
}


// Кодування input повідомлення у формат base64 (base64 нормально зберігатиметься в json)
void encode_base64(const unsigned char *input, int input_len, char *output, int *output_len) {
  *output_len = EVP_EncodeBlock((unsigned char*) output, input, input_len);
}
