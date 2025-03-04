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

void cmd_type_handler(client_t *client, const char *type)
{
    if (strcmp(type, "I") != 0)
        exit(-1);
    write_msg_to_client(client->control_fd, "200 Type set to I.");
}

void cmd_noop_handler(client_t *client, const char *_)
{
    write_msg_to_client(client->control_fd, "200 'NOOP' OK.");
}

void cmd_syst_handler(client_t *client, const char *_)
{
    write_msg_to_client(client->control_fd, "215 UNIX system type.");
}

static bool print_help_msg(const client_t *client, const char *args,
    const command_t *command)
{
    char buff[BUFSIZ] = {0};

    if (strcmp(command->name, args) != 0)
        return false;
    snprintf(buff, BUFSIZ, "214 %6s - %s", command->name,
        command->desc);
    write_msg_to_client(client->control_fd, buff);
    return true;
}

void cmd_help_handler(client_t *client, const char *args)
{
    char buff[BUFSIZ] = {0};

    if (args[1] == '\0') {
        for (size_t i = 0; i < COMMANDS_SIZE; i++) {
            memset(buff, 0, BUFSIZ);
            snprintf(buff, BUFSIZ, "214 %6s - %s", COMMANDS[i].name,
                COMMANDS[i].desc);
            write_msg_to_client(client->control_fd, buff);
        }
        return;
    }
    for (size_t i = 0; i < COMMANDS_SIZE; i++)
        if (print_help_msg(client, args, &COMMANDS[i]))
            return;
    write_msg_to_client(client->control_fd,
        "214 No description available.");
}
