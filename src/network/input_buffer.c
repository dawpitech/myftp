/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** input_buffer.c
*/

#include <stdio.h>
#include <string.h>

#include "network.h"

static void parse_client_buffer(client_t *client, char *buff_not_read)
{
    const size_t buff_len = strlen(client->cmd_buffer);
    size_t idx = 0;

    for (; idx < buff_len; idx++) {
        if (client->cmd_buffer[idx] == '\r' && idx + 1 < buff_len
            && client->cmd_buffer[idx + 1] == '\n') {
            idx += 2;
            strncpy(client->cmd, client->cmd_buffer, idx - 1);
            client->cmd[idx - 1] = '\0';
            client->cmd[idx - 2] = '\0';
            client->should_be_processed = true;
            break;
            }
    }
    if (idx < buff_len)
        strcat(buff_not_read, &client->cmd_buffer[idx]);
    if (!client->should_be_processed)
        strcat(buff_not_read, client->cmd_buffer);
}

void parse_client_input(client_t *client)
{
    char buff_not_read[BUFSIZ] = {0};

    memset(client->cmd, 0, BUFSIZ);
    parse_client_buffer(client, buff_not_read);
    memset(client->cmd_buffer, 0, BUFSIZ);
    strcpy(client->cmd_buffer, buff_not_read);
    client->cmd_buffer_offset = strlen(buff_not_read);
}

void process_client(client_t *client)
{
    bool cmd_found = false;

    for (size_t i = 0; i < COMMANDS_SIZE && !cmd_found; i++)
        if (client_cmd_handler(&COMMANDS[i], client->cmd, client) == true) {
            cmd_found = true;
        }
    if (!cmd_found)
        write_msg_to_client(client->control_fd, "500 Syntax Error");
    client->should_be_processed = false;
    parse_client_input(client);
}
