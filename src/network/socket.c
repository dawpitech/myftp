/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** socket.c
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>

#include "network.h"

static const char *mode_str(const data_modes_e_t mode)
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

void close_data_sock(client_t *client)
{
    printf("[INFO] Closing %s data connection.\n",
        mode_str(client->data_mode));
    if (client->data_fd != -1)
        close(client->data_fd);
    if (client->data_fd != -1)
        close(client->data_trf_fd);
    client->data_fd = -1;
    client->data_trf_fd = -1;
    client->data_mode = UNKNOWN;
}

int accept_data_sock(client_t *client)
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
