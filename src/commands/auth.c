/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** auth.c
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "auth.h"
#include "network.h"
#include "server.h"

static const auth_t *get_id_from_username(const char *username)
{
    for (size_t i = 0; i < IDS_SIZE; i++)
        if (strcmp(IDS[i].username, username) == 0)
            return &IDS[i];
    return NULL;
}

void cmd_user_handler(client_t *client, const char *username)
{
    const auth_t *id = get_id_from_username(username);

    if (id == NULL) {
        write_msg(client, "331", "Password required");
        return;
    }
    strcpy(client->username, username);
    if (id->password_needed) {
        write_msg(client, "331", "Password required");
        return;
    }
    client->is_auth = true;
    printf("[INFO] USER %s LOGGED IN\n", client->username);
    write_msg(client, "230", "User logged in, proceed.");
}

void cmd_pass_handler(client_t *client, const char *password)
{
    const auth_t *id = get_id_from_username(client->username);

    if (id == NULL || strcmp(id->password, password) != 0) {
        write_msg(client, "530", "Not logged in.");
        return;
    }
    client->is_auth = true;
    write_msg(client, "230", "User logged in, proceed.");
}

void cmd_quit_handler(client_t *client,
    __attribute__((unused)) const char *_)
{
    if (client->data_mode != UNKNOWN)
        close_data_sock(client);
    close(client->control_fd);
    memset(client, 0, sizeof(client_t));
    printf("[INFO] Client connection closed\n");
}
