/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** data.c
*/

#include "server.h"

void cmd_pasv_handler(client_t *client, __attribute__((unused)) const char *_)
{
    write_msg_to_client(client->control_fd,
    "227 Entering Passive Mode (127,0,0,1,4,26)");
}
