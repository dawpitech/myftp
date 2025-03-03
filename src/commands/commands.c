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

#include "../../include/server.h"

//TODO: check return value
void write_msg_to_client(const int client_control_fd, const char *str)
{
    (void) !write(client_control_fd, str, strlen(str));
    (void) !write(client_control_fd, CRLF, strlen(CRLF));
    printf("[ -> ] %s\n", str);
}

void cmd_type_handler(const client_t *client, const char *type)
{
    if (strcmp(type, "I") != 0)
        exit(-1);
    write_msg_to_client(client->control_fd, "200 Type set to I.");
}

void cmd_noop_handler(const client_t *client, const char *_)
{
    write_msg_to_client(client->control_fd, "200 'NOOP' OK.");
}
