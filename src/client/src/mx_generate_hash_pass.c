#include "../../inc/main.h"

char *mx_generate_password_hash(const char *password) {
    unsigned char sha1_hash[SHA_DIGEST_LENGTH];

    SHA1((const unsigned char *)password, mx_strlen(password), sha1_hash);

    char *hash_str = mx_strnew((SHA_DIGEST_LENGTH * 2) +1);
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf(&hash_str[i * 2], "%02x", (unsigned int)sha1_hash[i]);
    
    return hash_str;
}
