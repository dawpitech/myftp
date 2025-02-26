/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** server.c
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <bits/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>

#include "server.h"

static client_t *get_empty_client_slot(server_t *server)
{
    for (int i = 0; i < SERVER_MAX_CLIENTS; i++)
        if (server->clients[i].control_fd == 0)
            return &server->clients[i];
    return NULL;
}

static void searching_new_clients(server_t *server)
{
    const struct sockaddr_in client_addr = {0};
    socklen_t client_len = sizeof(client_addr);
    client_t *client;
    struct pollfd server_poll = {
        .fd = server->server_fd,
        .events = POLLIN
    };

    if (poll(&server_poll, 1, 10) != 0) {
        client = get_empty_client_slot(server);
        client->control_fd = accept(server->server_fd,
            (struct sockaddr*) &client_addr, &client_len);
        client->data_fd = -1;
        strcpy(client->username, "");
        strcpy(client->currPath, server->anonymous_default_path);
        printf("[INFO] New client connection\n");
        write_msg_to_client(client->control_fd, "220 Ready to server user.");
    }
}

static bool client_cmd_handler(const command_t *command, const char *buffer,
    client_t *client)
{
    if (strncmp(command->name, buffer, strlen(command->name)) != 0)
        return false;
    if (command->need_auth && !client->is_auth) {
        write_msg_to_client(client->control_fd, "530 Not logged in.");
        return true;
    }
    command->handler(client, buffer + strlen(command->name) + 1);
    return true;
}

static void reply_client(client_t *client)
{
    char buffer[BUFSIZ] = {0};
    unsigned long buff_len;
    bool cmd_found = false;

    if (read(client->control_fd, buffer, BUFSIZ) < 1)
        return;
    printf("[ <- ] %s", buffer);
    buff_len = strlen(buffer);
    buffer[buff_len - 2] = '\0';
    buffer[buff_len - 1] = '\0';
    for (size_t i = 0; i < COMMANDS_SIZE; i++)
        if (client_cmd_handler(&COMMANDS[i], buffer, client) == true) {
            cmd_found = true;
            break;
        }
    if (!cmd_found)
        write_msg_to_client(client->control_fd, "502 Not yet implemented");
}

static void update_client_data_fd(client_t *client)
{
    socklen_t client_len = sizeof(client->data_sock);
    struct pollfd data_poll = {
        .fd = client->data_fd,
        .events = POLLIN
    };

    if (poll(&data_poll, 1, 10) == 0)
        return;
    client->data_trf_fd = accept(client->data_fd,
            (struct sockaddr*) &client->data_sock, &client_len);
}

void events_loop(server_t *server)
{
    client_t *client;
    struct pollfd client_poll = {0};

    searching_new_clients(server);
    for (int x = 0; x < SERVER_MAX_CLIENTS
        && server->clients[x].control_fd != 0; x++) {
        client = &server->clients[x];
        client_poll.fd = client->control_fd;
        client_poll.events = POLLIN;
        update_client_data_fd(client);
        if (poll(&client_poll, 1, 10) == 0)
            continue;
        reply_client(client);
    }
}

int init_server(server_t *server)
{
    const int reuse = 1;
    const struct sockaddr_in socket_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(server->port),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    server->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_fd == -1)
        return -1;
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR,
        &reuse, sizeof(reuse)) == -1)
        return -1;
    if (bind(server->server_fd, (struct sockaddr*) &socket_addr,
        sizeof(socket_addr)) == -1)
        return -1;
    if (listen(server->server_fd, SERVER_MAX_CLIENTS) == -1)
        return -1;
    return 0;
}
