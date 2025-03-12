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

void panic_close_client(client_t *client)
{
    if (client->data_fd > 0)
        close(client->data_fd);
    if (client->data_trf_fd > 0)
        close(client->data_trf_fd);
    if (client->control_fd > 0)
        close(client->control_fd);
    memset(client, 0, sizeof(client_t));
    printf("[INFO] Connection died, closing\n");
}

int write_welcome(const client_t *client)
{
    const char *msg = "220 Ready to serve user." CRLF;
    const size_t msg_len = strlen(msg);

    if (write(client->control_fd, msg, msg_len) <= 0)
        return -1;
    return 0;
}

int write_msg(client_t *client, const char *code, const char *fmt, ...)
{
    va_list printf_args;
    va_list logs_args_cpy;

    va_start(printf_args, fmt);
    va_copy(logs_args_cpy, printf_args);
    if (!(dprintf(client->control_fd, "%s ", code) > 0
        && vdprintf(client->control_fd, fmt, printf_args) > 0
        && dprintf(client->control_fd, "%s", CRLF) > 0)) {
        panic_close_client(client);
        return -1;
    }
    va_end(printf_args);
    printf("[ -> ] %s ", code);
    vprintf(fmt, logs_args_cpy);
    printf("%s", CRLF);
    va_end(logs_args_cpy);
    return 0;
}
