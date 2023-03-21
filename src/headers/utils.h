#ifndef MY_UTILS_H
#define MY_UTILS_H
#include "cmd.h"
#include <stdbool.h>
#define DBG(...)                                                               \
    fprintf(stderr, "Error\nFile: %s\nLine: %d\nFunction: %s\n", __FILE__,     \
            __LINE__, __FUNCTION__);                                           \
    fprintf(stderr, "%s\n", __VA_ARGS__);

extern bool is_redir_sign(char c);
extern char *trim(char *str);
extern bool is_whitespace(char c);
extern void init_str_arr(char **ptr, const int arr_len, const int str_len);
extern int resize_cmd_len(const int cur_num, const int cur_len, const Cmd *cmd,
                          int max_cmd_len);
extern int resize_cmd_set(const int cur_num, Cmd *cmd, int max_cmd_len);
extern int calc_cmd_frag(const char *ptr);
extern void split_cmd(char *cmd_str, char **arr);
extern void free_str_arr(char **ptr, const int arr_len);
#endif
