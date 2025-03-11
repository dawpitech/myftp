/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** utils.c
*/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

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

void write_templated_msg(const client_t *client, const char *template)
{
    long rt_val1 = 0;
    long rt_val2 = 0;

    rt_val1 = write(client->control_fd, template, strlen(template));
    rt_val2 = write(client->control_fd, CRLF, strlen(CRLF));
    printf("[ -> ] %s\n", template);
    if (rt_val1 == -1 || rt_val2 == -1)
        fprintf(stderr, "[ERR!] Couldn't write message properly!\n");
}
