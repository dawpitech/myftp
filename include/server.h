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
    #include <linux/limits.h>
    #include <netinet/ip.h>

    #define CRLF "\r\n"

    #define SERVER_MAX_CLIENTS 128

typedef enum {
    UNKNOWN,
    ACTIVE,
    PASSIVE
} data_modes_e_t;

typedef struct client_s {
    char username[MAX_USERNAME];
    char currPath[PATH_MAX];
    int control_fd;
    struct sockaddr_in control_sock;
    int data_fd;
    struct sockaddr_in data_sock;
    int data_trf_fd;
    data_modes_e_t data_mode;
    bool is_auth;
    char home[PATH_MAX];
    char cmd_buffer[BUFSIZ];
    size_t cmd_buffer_offset;
    char cmd[BUFSIZ];
    bool should_be_processed;
} client_t;

typedef struct {
    int server_fd;
    unsigned short port;
    char anonymous_default_path[PATH_MAX];
    client_t clients[SERVER_MAX_CLIENTS];
} server_t;

typedef struct {
    char name[32];
    void (*handler)(client_t *client, const char *params);
    bool need_auth;
    char desc[64];
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
void cmd_list_handler(client_t *client, const char *args);
void cmd_retr_handler(client_t *client, const char *args);
void cmd_cdup_handler(client_t *client, const char *_);
void cmd_cwd_handler(client_t *client, const char *args);
void cmd_noop_handler(client_t *client, const char *_);
void cmd_syst_handler(client_t *client, const char *_);
void cmd_stor_handler(client_t *client, const char *args);
void cmd_help_handler(client_t *client, const char *args);
void cmd_port_handler(client_t *client, const char *args);
void cmd_dele_handler(client_t *client, const char *args);

static const command_t COMMANDS[] = {
    {"USER", cmd_user_handler, false,
        "Specify user for authentication"},
    {"PASS", cmd_pass_handler, false,
        "Specify password for authentication"},
    {"QUIT", cmd_quit_handler, false,
        "Drop current connection"},
    {"PWD", cmd_pwd_handler, true,
        "Print current path"},
    {"TYPE", cmd_type_handler, true,
        "Change formatting type of outputs"},
    {"PASV", cmd_pasv_handler, true,
        "Enable passive mode for data transfer"},
    {"LIST", cmd_list_handler, true,
        "List files in the current working directory"},
    {"RETR", cmd_retr_handler, true,
        "Retrieve a file from the server"},
    {"CDUP", cmd_cdup_handler, true,
        "Change working directory to parent directory"},
    {"CWD", cmd_cwd_handler, true,
        "Change working directory"},
    {"NOOP", cmd_noop_handler, false,
        "Do nothing"},
    {"HELP", cmd_help_handler, false,
        "Print this help message"},
    {"DELE", cmd_dele_handler, true,
        "Delete a file on the server"},
    {"PORT", cmd_port_handler, true,
        "Enable active mode for data transfer"},
    {"STOR", cmd_stor_handler, true,
        "upload a file on the server"},
    {"SYST", cmd_syst_handler, false,
        "Print server system info"},
    {"LPRT", NULL, true, "Not yet implemented"}
};
static const size_t COMMANDS_SIZE = sizeof(COMMANDS) / sizeof(command_t);
#endif //SERVER_H
