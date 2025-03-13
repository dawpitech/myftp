/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** data.c
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network.h"
#include "server.h"

static bool is_path_allowed(char const *home, char const *newpath)
{
    if (newpath == NULL)
        return false;
    return strncmp(home, newpath, strlen(home)) == 0;
}

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
    return WIFEXITED(pclose(ls)) == true;
}

void cmd_list_handler(client_t *client, const char *args)
{
    accept_data_sock(client);
    if (client->data_fd == -1 || client->data_trf_fd == -1) {
        write_msg(client, "425", "Rejecting data connection.");
        return;
    }
    if (write_msg(client, "150",
        "Data connection already open; starting transfer.") == -1)
        return;
    if (ls_to_client(client, args) != 0)
        write_msg(client, "425", "Requested file action not taken.");
    else
        write_msg(client, "226", "Closing data connection.");
    close_data_sock(client);
}

static int verify_content(const FILE *content, client_t *client)
{
    if (content == NULL) {
        write_msg(client, "550", "Requested action not taken.");
        return -1;
    }
    return write_msg(client, "150",
        "Data connection already open; starting transfer.");
}

static void write_file_to_client(client_t *client, const char *args)
{
    FILE *content;
    int chr;
    char filename_buff[BUFSIZ];

    snprintf((char *) &filename_buff, BUFSIZ, "%s/%s",
        client->currPath, args);
    content = fopen(filename_buff, "r");
    if (verify_content(content, client) == -1)
        exit(0);
    chr = fgetc(content);
    while (chr != EOF) {
        write(client->data_trf_fd, &chr, 1);
        chr = fgetc(content);
    }
    write_msg(client, "226", "Closing data connection.");
    close_data_sock(client);
    exit(0);
}

void cmd_retr_handler(client_t *client, const char *args)
{
    int pid;

    accept_data_sock(client);
    if (client->data_fd == -1 || client->data_trf_fd == -1) {
        write_msg(client, "425", "Rejecting data connection.");
        return;
    }
    pid = fork();
    if (pid == 0)
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
        write_msg(client, "425", "Rejecting data connection.");
        return;
    }
    if (write_msg(client, "150",
        "Data connection already open; starting transfer.") == -1)
        return;
    copy_file_content(client, args);
    if (write_msg(client, "226", "Closing data connection.") == -1)
        return;
    close_data_sock(client);
}

static void compute_dele_path(char *path_buffer, const char *given_path,
    const client_t *client)
{
    if (given_path[0] == '/') {
        strcpy(path_buffer, given_path);
        return;
    }
    snprintf(path_buffer, BUFSIZ, "%s/%s",
        client->currPath, given_path);
}

void cmd_dele_handler(client_t *client, const char *args)
{
    char path_buff[BUFSIZ] = {0};
    char *dele_path;

    compute_dele_path(path_buff, args, client);
    dele_path = realpath(path_buff, NULL);
    if (!is_path_allowed(client->home, dele_path)) {
        write_msg(client, "550", "Requested action not taken.");
        return;
    }
    if (unlink(path_buff) == -1) {
        write_msg(client, "550", "Requested action not taken.");
    } else {
        write_msg(client, "250",
            "250 Requested file action okay, completed.");
    }
    free(dele_path);
}
