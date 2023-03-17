#include "headers/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_redir_sign(char c) {
    return c == '|' || c == '<' || c == '>' || c == '&';
}

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

char *trim(char *str) {
    char *end = str + strlen(str);
    puts(str);
    while (is_whitespace(*str)) {
        ++str;
    }
    while (is_whitespace(*end)) {
        *end = '\0';
        --end;
    }
    return str;
}

void init_str_arr(char **ptr, int arr_len, int str_len) {
    for (int i = 0; i < arr_len; i++) {
        ptr[i] = (char *)malloc(str_len);
        memset(ptr[i], '\0', str_len);
    }
}
