/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** commands.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network.h"
#include "server.h"

//TODO: check return value
/*void write_msg_to_client(const int client_control_fd, const char *str)
{
    (void) !write(client_control_fd, str, strlen(str));
    (void) !write(client_control_fd, CRLF, strlen(CRLF));
    printf("[ -> ] %s\n", str);
}*/

void cmd_type_handler(client_t *client, const char *type)
{
    if (strcmp(type, "I") != 0)
        exit(-1);
    write_msg(client, "200", "Type set to I.");
}

void cmd_noop_handler(client_t *client, __attribute__((unused)) const char *_)
{
    write_msg(client, "200", "'NOOP' OK.");
}

void cmd_syst_handler(client_t *client, __attribute__((unused)) const char *_)
{
    write_msg(client, "215", "UNIX system type.");
}

static bool print_help_msg(client_t *client, const char *args,
    const command_t *command)
{
    if (strcmp(command->name, args) != 0)
        return false;
    write_msg(client, "214", "%6s -%s", command->name, command->desc);
    return true;
}

static void print_full_help(client_t *client)
{
    for (size_t i = 0; i < COMMANDS_SIZE; i++)
        if (write_msg(client, "214", "%6s - %s", COMMANDS[i].name,
            COMMANDS[i].desc) == -1)
            break;
}

void cmd_help_handler(client_t *client, const char *args)
{
    if (args[1] == '\0') {
        print_full_help(client);
        return;
    }
    for (size_t i = 0; i < COMMANDS_SIZE; i++)
        if (print_help_msg(client, args, &COMMANDS[i]))
            return;
    write_msg(client, "214", "No description available.");
}
