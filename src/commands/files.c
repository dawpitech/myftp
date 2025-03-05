/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** data.c
*/

#include <stdio.h>
#include <unistd.h>

#include "server.h"

static void ls_to_client(const client_t *client, const char *args)
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
}

void cmd_list_handler(client_t *client, const char *args)
{
    (void) !args;
    if (client->data_fd == -1 || client->data_trf_fd == -1) {
        write_msg_to_client(client->control_fd,
            "425 Rejecting data connection.");
        return;
    }
    write_msg_to_client(client->control_fd,
        "150 Data connection already open; starting transfer.");
    ls_to_client(client, args);
    printf("[INFO] Closing data transfer socket.\n");
    write_msg_to_client(client->control_fd, "226 Closing data connection.");
    close(client->data_fd);
    close(client->data_trf_fd);
    client->data_fd = -1;
    client->data_trf_fd = -1;
}

static void write_file_to_client(const client_t *client, const char *args)
{
    FILE *content;
    int chr;
    char filename_buff[BUFSIZ];

    snprintf((char *) &filename_buff, BUFSIZ, "%s/%s",
        client->currPath, args);
    content = fopen(filename_buff, "r");
    chr = fgetc(content);
    while (chr != EOF) {
        write(client->data_trf_fd, &chr, 1);
        chr = fgetc(content);
    }
}

void cmd_retr_handler(client_t *client, const char *args)
{
    (void) !args;
    if (client->data_fd == -1 || client->data_trf_fd == -1) {
        write_msg_to_client(client->control_fd,
            "425 Rejecting data connection.");
        return;
    }
    write_msg_to_client(client->control_fd,
        "150 Data connection already open; starting transfer.");
    write_file_to_client(client, args);
    printf("[INFO] Closing data transfer socket.\n");
    write_msg_to_client(client->control_fd, "226 Closing data connection.");
    close(client->data_fd);
    close(client->data_trf_fd);
    client->data_fd = -1;
    client->data_trf_fd = -1;
}

static void copy_file_content(const client_t *client, const char *arg)
{
    char filename_buff[BUFSIZ];
    char chr;
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
    if (client->data_fd == -1 || client->data_trf_fd == -1) {
        write_msg_to_client(client->control_fd,
            "425 Rejecting data connection.");
        return;
    }
    write_msg_to_client(client->control_fd,
    "150 Data connection already open; starting transfer.");
    copy_file_content(client, args);
    printf("[INFO] Closing data transfer socket.\n");
    write_msg_to_client(client->control_fd, "226 Closing data connection.");
    close(client->data_fd);
    close(client->data_trf_fd);
    client->data_fd = -1;
    client->data_trf_fd = -1;
}
