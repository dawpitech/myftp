/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** ntw_utils.h
*/

#ifndef NTW_UTILS_H
    #define NTW_UTILS_H

    #include "server.h"

void write_msg(const client_t *client, const char *code, const char *fmt,
    ...);
void close_data(client_t *client);
int accept_passive_data(client_t *client);
#endif //NTW_UTILS_H
