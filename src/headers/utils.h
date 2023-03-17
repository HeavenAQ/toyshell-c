#ifndef MY_UTILS_H
#define MY_UTILS_H
#include <stdbool.h>
#define DBG(...)                                                               \
    fprintf(stderr, "Error\nFile: %s\nLine: %d\nFunction: %s\n", __FILE__,     \
            __LINE__, __FUNCTION__);                                           \
    fprintf(stderr, "%s\n", __VA_ARGS__);

bool is_redir_sign(char c);
char *trim(char *str);
bool is_whitespace(char c);
void init_str_arr(char **ptr, int arr_len, int str_len);
#endif
