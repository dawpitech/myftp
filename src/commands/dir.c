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

#include "server.h"

static bool is_path_allowed(char const *home, char const *newpath)
{
    char const *real_newpath = realpath(newpath, NULL);

    if (real_newpath == NULL)
        return false;
    return strncmp(home, real_newpath, strlen(home)) == 0;
}

void cmd_pwd_handler(client_t *client, __attribute__((unused)) const char *_)
{
    char buffer[512];

    sprintf(buffer, "257 %s", realpath(client->currPath, NULL));
    write_msg_to_client(client->control_fd, buffer);
}

void cmd_cdup_handler(client_t *client, const char *_)
{
    cmd_cwd_handler(client, "..");
}

static int check_for_path_errors(const char *args, const char *realpath_buff,
    const client_t *client)
{
    if (strlen(args) == 0) {
        write_msg_to_client(client->control_fd,
            "550 Requested action not taken.");
        return -1;
    }
    if (realpath_buff == NULL) {
        write_msg_to_client(client->control_fd, "550 Error occurred.");
        return -1;
    }
    if (!is_path_allowed(client->home, realpath_buff)) {
        write_msg_to_client(client->control_fd,
            "550 Requested action not taken.");
        return -1;
    }
    return 0;
}

void cmd_cwd_handler(client_t *client, const char *args)
{
    char new_path[PATH_MAX] = {0};
    char *realpath_buff;

    if (args[0] == '/') {
        realpath_buff = realpath(args, NULL);
    } else {
        strcpy(new_path, client->currPath);
        strcat(new_path, "/");
        strcat(new_path, args);
        realpath_buff = realpath(new_path, NULL);
        if (check_for_path_errors(args, realpath_buff, client) == -1)
            return;
    }
    strcpy(client->currPath, realpath_buff);
    free(realpath_buff);
    write_msg_to_client(client->control_fd,
        "250 Requested file action okay, completed.");
}
