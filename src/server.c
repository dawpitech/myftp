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

    client = get_empty_client_slot(server);
    client->control_fd = accept(server->server_fd,
        (struct sockaddr*) &client_addr, &client_len);
    init_client(client, server);
    strcpy(client->currPath, realpath(server->anonymous_default_path,
        NULL));
    printf("[INFO] New client connection\n");
    write_welcome(client);
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

static void add_to_poll(poll_config_t *poll_config,
    const int client_fd, client_t *client)
{
    poll_config->polls[poll_config->size].fd = client_fd;
    poll_config->polls[poll_config->size].events = POLLIN;
    if (client != NULL)
        poll_config->clients[poll_config->size] = client;
    poll_config->size += 1;
}

void events_loop(server_t *server)
{
    poll_config_t poll_config = {0};

    add_to_poll(&poll_config, server->server_fd, NULL);
    for (int i = 0; i < SERVER_MAX_CLIENTS; i++) {
        if (server->clients[i].control_fd == 0)
            continue;
        add_to_poll(&poll_config, server->clients[i].control_fd,
            &server->clients[i]);
    }
    poll(poll_config.polls, poll_config.size, -1);
    for (size_t i = 0; i < poll_config.size; i++) {
        if (poll_config.polls[i].revents == 0)
            continue;
        if (i == 0)
            searching_new_clients(server);
        else
            reply_client(poll_config.clients[i]);
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
