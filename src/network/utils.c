/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** utils.c
*/

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>

#include "ntw_utils.h"

static const char*mode_str(const data_modes_e mode)
{
    switch (mode) {
        default:
        case UNKNOWN:
            return "UNKNOWN";
        case ACTIVE:
            return "ACTIVE";
        case PASSIVE:
            return "PASSIVE";
    }
}

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

void close_data(client_t *client)
{
    printf("[INFO] Closing %s data connection.\n", mode_str(client->data_mode));
    if (client->data_fd != -1)
        close(client->data_fd);
    if (client->data_fd != -1)
        close(client->data_trf_fd);
    client->data_fd = -1;
    client->data_trf_fd = -1;
    client->data_mode = UNKNOWN;
}

int accept_passive_data(client_t *client)
{
    socklen_t client_len = sizeof(client->data_sock);
    struct pollfd data_poll = {
        .fd = client->data_fd,
        .events = POLLIN
    };

    if (client->data_mode != PASSIVE)
        return 0;
    if (poll(&data_poll, 1, 1) == 0)
        return -1;
    client->data_trf_fd = accept(client->data_fd,
        (struct sockaddr*) &client->data_sock, &client_len);
    return 0;
}
