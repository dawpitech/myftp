/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** control.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "network.h"
#include "server.h"

static void handle_addr_token(const char *token, const int idx, char *ip,
    unsigned short *port)
{
    if (idx < 4) {
        if (idx != 0)
            strcat(ip, ".");
        strcat(ip, token);
    }
    if (idx == 4)
        *port = strtoul(token, NULL, 10);
    if (idx == 5)
        *port = *port * 256 + strtoul(token, NULL, 10);
}

static void parse_client_addr(char *ip, unsigned short *port,
    const char *raw_args, client_t *client)
{
    char *token;
    char arg[BUFSIZ] = {0};

    strcpy(arg, raw_args);
    token = strtok(arg, ",");
    for (int i = 0; i < 6; i++) {
        if (token == NULL) {
            write_msg(client, "530", "Syntax Error");
            return;
        }
        handle_addr_token(token, i, ip, port);
        token = strtok(NULL, ",");
    }
}

void cmd_port_handler(client_t *client, const char *args)
{
    char ip[256] = {0};
    unsigned short port = 0;

    parse_client_addr(ip, &port, args, client);
    client->data_sock.sin_family = AF_INET;
    client->data_sock.sin_port = htons(port);
    client->data_sock.sin_addr.s_addr = inet_addr(ip);
    client->data_fd = socket(AF_INET, SOCK_STREAM, 0);
    client->data_trf_fd = client->data_fd;
    if (connect(client->data_fd, (struct sockaddr*) &client->data_sock,
    sizeof(client->data_sock)) == -1) {
        write_msg(client, "421", "Service not available.");
        return;
    }
    printf("[INFO] Connected to remote %s:%d\n", ip, port);
    client->data_mode = ACTIVE;
    write_msg(client, "200", "OK.");
}

static void pasv_client_setup(client_t *client)
{
    client->data_sock.sin_family = AF_INET;
    client->data_sock.sin_port = htons(0);
    client->data_sock.sin_addr.s_addr = htonl(INADDR_ANY);
    client->data_fd = socket(AF_INET, SOCK_STREAM, 0);
}

void cmd_pasv_handler(client_t *client, __attribute__((unused)) const char *_)
{
    socklen_t len = sizeof(client->data_sock);

    pasv_client_setup(client);
    if (bind(client->data_fd, (struct sockaddr*) &client->data_sock,
        sizeof(client->data_sock)) == -1) {
        write_msg(client, "421", "Service not available.");
        return;
        }
    getsockname(client->data_fd, (struct sockaddr*) &client->data_sock, &len);
    listen(client->data_fd, 1);
    printf("[INFO] Port %d opened for passive data transport\n",
        ntohs(client->data_sock.sin_port));
    client->data_mode = PASSIVE;
    write_msg(client, "227", "Entering Passive Mode (127,0,0,1,%d,%d)",
        ntohs(client->data_sock.sin_port) / 256,
        ntohs(client->data_sock.sin_port) % 256);
}
