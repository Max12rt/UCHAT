#include "../../../inc/uchat.h"

int mx_get_substring(char *str) {
    char *comma = mx_strchr(str, ',');
    size_t len = mx_strlen(comma + 1);
    char *dst = malloc(sizeof(char) * len + 1);
    
    mx_strncpy(dst, comma + 1, len);
    dst[len] = '\0';
    return mx_atoi(dst);
}
