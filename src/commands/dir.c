/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** dir.c
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

#include "network.h"
#include "server.h"

static bool is_path_allowed(char const *home, char const *newpath)
{
    char real_newpath[PATH_MAX];

    if (realpath(newpath, real_newpath) == NULL)
        return false;
    return strncmp(home, real_newpath, strlen(home)) == 0;
}

void cmd_pwd_handler(client_t *client, __attribute__((unused)) const char *_)
{
    char path_buff[PATH_MAX];

    realpath(client->currPath, path_buff);
    write_msg(client, "257", "%s", path_buff);
}

void cmd_cdup_handler(client_t *client, __attribute__((unused)) const char *_)
{
    cmd_cwd_handler(client, "..");
}

static int check_for_path_errors(const char *args, const char *realpath_buff,
    client_t *client)
{
    if (strlen(args) == 0)
        return write_msg(client, "550", "Requested action not taken.");
    if (realpath_buff == NULL)
        return write_msg(client, "550", "Error occurred.");
    if (!is_path_allowed(client->home, realpath_buff))
        return write_msg(client, "550", "Requested action not taken.");
    return 0;
}

void cmd_cwd_handler(client_t *client, const char *args)
{
    char new_path[PATH_MAX] = {0};
    char realpath_buff[PATH_MAX] = {0};

    if (args[0] == '/') {
        realpath(args, realpath_buff);
    } else {
        strcpy(new_path, client->currPath);
        strcat(new_path, "/");
        strcat(new_path, args);
        realpath(new_path, realpath_buff);
        if (check_for_path_errors(args, realpath_buff, client) == -1)
            return;
    }
    strcpy(client->currPath, realpath_buff);
    write_msg(client, "250", "Requested file action okay, completed.");
}
