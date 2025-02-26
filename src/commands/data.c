/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** data.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "server.h"

void cmd_pasv_handler(client_t *client, __attribute__((unused)) const char *_)
{
    char msg_buf[BUFSIZ];
    socklen_t len = sizeof(client->data_sock);

    client->data_sock.sin_family = AF_INET;
    client->data_sock.sin_port = htons(0);
    client->data_sock.sin_addr.s_addr = htonl(INADDR_ANY);
    client->data_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(client->data_fd, (struct sockaddr*) &client->data_sock,
        sizeof(client->data_sock)) == -1) {
        write_msg_to_client(client->control_fd, "421 Service not available.");
        return;
    }
    getsockname(client->data_fd, (struct sockaddr*) &client->data_sock, &len);
    listen(client->data_fd, 1);
    printf("[INFO] Port %d opened for client data transport\n",
        ntohs(client->data_sock.sin_port));
    snprintf((char *) &msg_buf, BUFSIZ, "227 Entering Passive Mode "
        "(127,0,0,1,%d,%d)", ntohs(client->data_sock.sin_port / 256),
        ntohs(client->data_sock.sin_port % 256));
    write_msg_to_client(client->control_fd, msg_buf);
}

void ls_child(const client_t *client, const char *args)
{
    FILE *ls = popen("ls -l", "r");
    int chr = fgetc(ls);

    while (chr != EOF) {
        write(client->data_trf_fd, &chr, 1);
        chr = fgetc(ls);
    }
    exit(0);
}

void cmd_list_handler(client_t *client, const char *args)
{
    pid_t pid;

    (void) !args;
    if (client->data_fd == -1) {
        write_msg_to_client(client->control_fd,
            "425 Rejecting data connection.");
        return;
    }
    write_msg_to_client(client->control_fd,
        "125 Data connection already open; starting transfer.");
    pid = fork();
    if (pid == 0)
        ls_child(client, args);
    waitpid(-pid, NULL, WUNTRACED);
    close(client->data_fd);
    close(client->data_trf_fd);
    client->data_fd = -1;
    printf("[INFO] Closing data transfer socket.\n");
    write_msg_to_client(client->control_fd, "226 Closing data connection.");
}

void retr_child(const client_t *client, const char *args)
{
    FILE *content = fopen("Makefile", "r");
    int chr = fgetc(content);

    while (chr != EOF) {
        write(client->data_trf_fd, &chr, 1);
        chr = fgetc(content);
    }
    exit(0);
}

void cmd_retr_handler(client_t *client, const char *args)
{
    pid_t pid;

    (void) !args;
    if (client->data_fd == -1) {
        write_msg_to_client(client->control_fd,
            "425 Rejecting data connection.");
        return;
    }
    write_msg_to_client(client->control_fd,
        "125 Data connection already open; starting transfer.");
    pid = fork();
    if (pid == 0)
        retr_child(client, args);
    waitpid(-pid, NULL, WUNTRACED);
    close(client->data_fd);
    close(client->data_trf_fd);
    client->data_fd = -1;
    printf("[INFO] Closing data transfer socket.\n");
    write_msg_to_client(client->control_fd, "226 Closing data connection.");
}
