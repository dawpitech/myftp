/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** server.h
*/

#ifndef SERVER_H
    #define SERVER_H

    #include <auth.h>
    #include <stdbool.h>
    #include <netinet/ip.h>

    #define CRLF "\r\n"

    #define SERVER_MAX_CLIENTS 32
    #define MAX_PATH 256

typedef struct {
    char username[MAX_USERNAME];
    char currPath[MAX_PATH];
    int control_fd;
    struct sockaddr_in control_sock;
    int data_fd;
    struct sockaddr_in data_sock;
    bool is_auth;
} client_t;

typedef struct {
    int server_fd;
    unsigned short port;
    char anonymous_default_path[MAX_PATH];
    client_t clients[SERVER_MAX_CLIENTS];
} server_t;

typedef struct {
    char name[32];
    void (*handler)(client_t *client, const char *params);
    bool need_auth;
} command_t;

int init_server(server_t *server);
void write_msg_to_client(int client_control_fd, const char *str);
void events_loop(server_t *server);

void cmd_user_handler(client_t *client, const char *username);
void cmd_pass_handler(client_t *client, const char *password);
void cmd_quit_handler(client_t *client, const char *_);
void cmd_pwd_handler(client_t *client, const char *_);
void cmd_type_handler(client_t *client, const char *type);
void cmd_pasv_handler(client_t *client, const char *_);

static const command_t COMMANDS[] = {
    {"USER", cmd_user_handler, false},
    {"PASS", cmd_pass_handler, false},
    {"QUIT", cmd_quit_handler, false},
    {"PWD", cmd_pwd_handler, true},
    {"TYPE", cmd_type_handler, true},
    {"PASV", cmd_pasv_handler, true},
};
static const size_t COMMANDS_SIZE = sizeof(COMMANDS) / sizeof(command_t);
#endif //SERVER_H
