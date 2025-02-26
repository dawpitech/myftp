/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** dir.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "server.h"

void cmd_pwd_handler(client_t *client, __attribute__((unused)) const char *_)
{
    char buffer[512];

    sprintf(buffer, "257 %s", realpath(client->currPath, NULL));
    write_msg_to_client(client->control_fd, buffer);
}
