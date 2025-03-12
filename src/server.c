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

static bool client_cmd_handler(const command_t *command, const char *buffer,
    client_t *client)
{
    if (strncasecmp(command->name, buffer, strlen(command->name)) != 0)
        return false;
    if (command->handler == NULL) {
        write_msg_to_client(client->control_fd, "502 Not yet implemented");
        return true;
    }
    if (command->need_auth && !client->is_auth) {
        write_msg_to_client(client->control_fd, "530 Not logged in.");
        return true;
    }
    command->handler(client, buffer + strlen(command->name) + 1);
    return true;
}

static void parse_client_buffer(client_t *client, char *buff_not_read)
{

    const size_t buff_len = strlen(client->cmd_buffer);
    size_t idx = 0;

    for (; idx < buff_len; idx++) {
        if (client->cmd_buffer[idx] == '\r' && idx + 1 < buff_len && client->cmd_buffer[idx + 1] == '\n') {
            idx += 2;
            strncpy(client->cmd, client->cmd_buffer, idx - 1);
            client->cmd[idx - 1] = '\0';
            client->cmd[idx - 2] = '\0';
            client->should_be_processed = true;
            break;
        }
    }
    if (idx < buff_len)
        strcat(buff_not_read, &client->cmd_buffer[idx]);
    if (!client->should_be_processed)
        strcat(buff_not_read, client->cmd_buffer);
}

static void parse_client_input(client_t *client)
{
    char buff_not_read[BUFSIZ] = {0};

    client->should_be_processed = false;
    memset(client->cmd, 0, BUFSIZ);
    parse_client_buffer(client, buff_not_read);
    memset(client->cmd_buffer, 0, BUFSIZ);
    strcpy(client->cmd_buffer, buff_not_read);
    client->cmd_buffer_offset = strlen(buff_not_read);
    //printf("[INFO] Current command: %s\n", client->cmd);
    //printf("[INFO] Command buffer: %s\n", client->cmd_buffer);
}

static void reply_client(client_t *client)
{
    bool cmd_found = false;

    //printf("Writing in buffer with offset of: %lu\n", client->cmd_buffer_offset);
    if (read(client->control_fd, client->cmd_buffer +
        client->cmd_buffer_offset, BUFSIZ - client->cmd_buffer_offset) < 1)
        return;
    printf("[ <- ] '%s'\n", client->cmd_buffer);
    parse_client_input(client);
    while (client->should_be_processed) {
        for (size_t i = 0; i < COMMANDS_SIZE && !cmd_found; i++)
            if (client_cmd_handler(&COMMANDS[i], client->cmd, client) == true)
                cmd_found = true;
        if (!cmd_found)
            write_msg_to_client(client->control_fd, "500 Syntax Error");
        parse_client_input(client);
    }
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
