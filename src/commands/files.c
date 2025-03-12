/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** data.c
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "network.h"
#include "server.h"

static int ls_to_client(const client_t *client, const char *args)
{
    FILE *ls;
    int chr;
    char shell_buff[BUFSIZ];

    snprintf((char *) &shell_buff, BUFSIZ, "ls -l %s/%s",
        client->currPath, args);
    ls = popen(shell_buff, "r");
    chr = fgetc(ls);
    while (chr != EOF) {
        write(client->data_trf_fd, &chr, 1);
        chr = fgetc(ls);
    }
    return WEXITSTATUS(pclose(ls));
}

void cmd_list_handler(client_t *client, const char *args)
{
    accept_data_sock(client);
    if (client->data_fd == -1 || client->data_trf_fd == -1) {
        write_msg_to_client(client->control_fd,
            "425 Rejecting data connection.");
        return;
    }
    write_msg_to_client(client->control_fd,
        "150 Data connection already open; starting transfer.");
    if (ls_to_client(client, args) != 0) {
        write_msg_to_client(client->control_fd,
            "450 Requested file action not taken.");
    } else {
        write_msg_to_client(client->control_fd,
            "226 Closing data connection.");
    }
    close_data_sock(client);
}

static int verify_content(const FILE *content, const client_t *client)
{
    if (content == NULL) {
        write_msg_to_client(client->control_fd,
        "550 Requested action not taken.");
        return -1;
    }
    write_msg_to_client(client->control_fd,
    "150 Data connection already open; starting transfer.");
    return 0;
}

static void write_file_to_client(const client_t *client, const char *args)
{
    FILE *content;
    int chr;
    char filename_buff[BUFSIZ];

    snprintf((char *) &filename_buff, BUFSIZ, "%s/%s",
        client->currPath, args);
    content = fopen(filename_buff, "r");
    if (verify_content(content, client) == -1)
        return;
    chr = fgetc(content);
    while (chr != EOF) {
        write(client->data_trf_fd, &chr, 1);
        chr = fgetc(content);
    }
    write_msg_to_client(client->control_fd, "226 Closing data connection.");
}

void cmd_retr_handler(client_t *client, const char *args)
{
    accept_data_sock(client);
    if (client->data_fd == -1 || client->data_trf_fd == -1) {
        write_msg_to_client(client->control_fd,
            "425 Rejecting data connection.");
        return;
    }
    write_file_to_client(client, args);
    close_data_sock(client);
}

static void copy_file_content(const client_t *client, const char *arg)
{
    char filename_buff[BUFSIZ];
    int chr;
    FILE *src;
    FILE *dest;

    snprintf((char *) &filename_buff, BUFSIZ, "%s/%s",
    client->currPath, arg);
    src = fdopen(client->data_trf_fd, "r");
    dest = fopen(filename_buff, "w");
    chr = fgetc(src);
    while (chr != EOF) {
        fputc(chr, dest);
        chr = fgetc(src);
    }
    fclose(dest);
    fclose(src);
}

void cmd_stor_handler(client_t *client, const char *args)
{
    accept_data_sock(client);
    if (client->data_fd == -1 || client->data_trf_fd == -1) {
        write_msg_to_client(client->control_fd,
            "425 Rejecting data connection.");
        return;
    }
    write_msg_to_client(client->control_fd,
    "150 Data connection already open; starting transfer.");
    copy_file_content(client, args);
    write_msg_to_client(client->control_fd, "226 Closing data connection.");
    close_data_sock(client);
}

void cmd_dele_handler(client_t *client, const char *args)
{
    errno = 0;
    if (unlink(args) == -1) {
        write_msg_to_client(client->control_fd,
            "550 Requested action not taken.");
    } else {
        write_msg_to_client(client->control_fd,
            "250 Requested file action okay, completed.");
    }
}
