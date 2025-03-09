/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** utils.c
*/

#include <stdarg.h>
#include <stdio.h>

#include "ntw_utils.h"

void write_msg(const client_t *client, const char *code, const char *fmt, ...)
{
    va_list printf_args;
    va_list logs_args_cpy;

    va_start(printf_args, fmt);
    va_copy(logs_args_cpy, printf_args);
    dprintf(client->control_fd, "%s ", code);
    vdprintf(client->control_fd, fmt, printf_args);
    dprintf(client->control_fd, "%s", CRLF);
    va_end(printf_args);
    printf("[ -> ] %s ", code);
    vprintf(fmt, logs_args_cpy);
    printf("%s", CRLF);
    va_end(logs_args_cpy);
}
