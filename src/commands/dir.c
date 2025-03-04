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

void compute_new_path(client_t *client, const char *path)
{
    char new_path[PATH_MAX] = {0};
    char *realpath_buff;

    if (path[0] == '/') {
        realpath_buff = realpath(path, NULL);
    } else {
        strcpy(new_path, client->currPath);
        strcat(new_path, "/");
        strcat(new_path, path);
        realpath_buff = realpath(new_path, NULL);
        if (realpath_buff == NULL) {
            write_msg_to_client(client->control_fd, "550 Error occurred.");
            return;
        }
    }
    strcpy(client->currPath, realpath_buff);
    free(realpath_buff);
}

void cmd_cwd_handler(client_t *client, const char *args)
{
    compute_new_path(client, args);
    write_msg_to_client(client->control_fd,
        "250 Requested file action okay, completed.");
}
