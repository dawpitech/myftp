/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** commands.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"

//TODO: check return value
void write_msg_to_client(const int client_control_fd, const char *str)
{
    (void) !write(client_control_fd, str, strlen(str));
    (void) !write(client_control_fd, CRLF, strlen(CRLF));
    printf("[ -> ] %s\n", str);
}

void cmd_quit_handler(client_t *client, __attribute__((unused)) const char *_)
{
    close(client->control_fd);
    if (client->data_fd != -1)
        close(client->data_fd);
    printf("[INFO] Client connection closed\n");
}

void cmd_type_handler(client_t *client, const char *type)
{
    if (strcmp(type, "I") != 0)
        exit(-1);
    write_msg_to_client(client->control_fd, "200 Type set to I.");
}
