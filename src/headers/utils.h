#ifndef MY_UTILS_H
#define MY_UTILS_H
#include "cmd.h"
#include <stdbool.h>
#define DBG(...)                                                               \
    fprintf(stderr, "Error\nFile: %s\nLine: %d\nFunction: %s\n", __FILE__,     \
            __LINE__, __FUNCTION__);                                           \
    fprintf(stderr, "%s\n", __VA_ARGS__);

bool is_redir_sign(char c);
char *trim(char *str);
bool is_whitespace(char c);
void init_str_arr(char **ptr, const int arr_len, const int str_len);
int resize_cmd_len(const int cur_num, const int cur_len, const Cmd *cmd,
                   int max_cmd_len);
int resize_cmd_set(const int cur_num, Cmd *cmd, int max_cmd_len);
int calc_cmd_frag(const char *ptr);
void split_cmd(char *cmd_str, char **arr);
#endif
