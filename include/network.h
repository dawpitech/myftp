/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** network.h
*/

#ifndef NTW_UTILS_H
    #define NTW_UTILS_H

    #include "server.h"

void write_msg(const client_t *client, const char *code, const char *fmt,
    ...);
void write_templated_msg(const client_t *client, const char *template);
void parse_client_input(client_t *client);
void process_client(client_t *client);

void close_data_sock(client_t *client);
int accept_data_sock(client_t *client);
#endif //NTW_UTILS_H
