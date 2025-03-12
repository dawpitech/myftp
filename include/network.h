/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** network.h
*/

#ifndef NTW_UTILS_H
    #define NTW_UTILS_H

    #include "server.h"

int write_welcome(const client_t *client);
int write_msg(client_t *client, const char *code, const char *fmt,
    ...);

void parse_client_input(client_t *client);
void process_client(client_t *client);

void panic_close_client(client_t *client);
void close_data_sock(client_t *client);
int accept_data_sock(client_t *client);
#endif //NTW_UTILS_H
