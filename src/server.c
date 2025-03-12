/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** server.c
*/

// ReSharper disable CppDFAConstantConditions
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <bits/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>

#include "server.h"
#include "network.h"

static client_t *get_empty_client_slot(server_t *server)
{
    for (int i = 0; i < SERVER_MAX_CLIENTS; i++)
        if (server->clients[i].control_fd == 0)
            return &server->clients[i];
    return NULL;
}

static void init_client(client_t *client, const server_t *server)
{
    client->data_fd = -1;
    client->data_trf_fd = -1;
    strcpy(client->username, "");
    strcpy(client->home, server->anonymous_default_path);
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

    if (poll(&server_poll, 1, 1) != 0) {
        client = get_empty_client_slot(server);
        client->control_fd = accept(server->server_fd,
            (struct sockaddr*) &client_addr, &client_len);
        init_client(client, server);
        strcpy(client->currPath, realpath(server->anonymous_default_path,
            NULL));
        printf("[INFO] New client connection\n");
        write_msg(client, "220", "Ready to serve user.");
    }
}

bool client_cmd_handler(const command_t *command, const char *buffer,
    client_t *client)
{
    if (strncasecmp(command->name, buffer, strlen(command->name)) != 0)
        return false;
    if (command->handler == NULL) {
        write_msg(client, "502", "Not yet implemented");
        return true;
    }
    if (command->need_auth && !client->is_auth) {
        write_msg(client, "530", "530 Not logged in.");
        return true;
    }
    command->handler(client, buffer + strlen(command->name) + 1);
    return true;
}

static void reply_client(client_t *client)
{
    const long readable_bytes = read(client->control_fd, client->cmd_buffer +
        client->cmd_buffer_offset, BUFSIZ - client->cmd_buffer_offset);

    if (readable_bytes <= 0) {
        panic_close_client(client);
        return;
    }
    printf("[ <- ] %s", client->cmd_buffer);
    parse_client_input(client);
    while (client->should_be_processed)
        process_client(client);
}

void events_loop(server_t *server)
{
    client_t *client;
    struct pollfd client_poll = {0};

    client_poll.events = POLLIN;
    searching_new_clients(server);
    for (int x = 0; x < SERVER_MAX_CLIENTS
        && server->clients[x].control_fd != 0; x++) {
        client = &server->clients[x];
        client_poll.fd = client->control_fd;
        if (poll(&client_poll, 1, 1) == 0)
            continue;
        reply_client(client);
    }
}

static int load_anon_home(server_t *server)
{
    char *homepath;

    homepath = realpath(server->anonymous_default_path, NULL);
    if (homepath == NULL) {
        fprintf(stderr, "myftp: invalid anonymous home path\n");
        return -1;
    }
    strcpy(server->anonymous_default_path, homepath);
    free(homepath);
    printf("[INFO] Account 'Anonymous' home is: %s\n",
        server->anonymous_default_path);
    return 0;
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
    if (server->server_fd == -1 || load_anon_home(server) == -1)
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
