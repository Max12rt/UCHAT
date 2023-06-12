#include "../inc/libmx.h"

char *mx_strndup(const char *s1, size_t n) {
    size_t s1_len = mx_strlen(s1);

    if (s1_len < n) n = s1_len;
    
    char *new_string = mx_strnew(n);
    return mx_strncpy(new_string, s1, n);
}
