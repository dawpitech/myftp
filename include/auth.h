/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** auth.h
*/

#ifndef AUTH_H
    #define AUTH_H

    #include <stdbool.h>
    #include <stddef.h>

    #define MAX_USERNAME 64
    #define MAX_PASSWORD 256

typedef struct {
    char username[MAX_USERNAME];
    bool password_needed;
    char password[MAX_PASSWORD];
} auth_t;

static const auth_t IDS[] = {
    {"Anonymous", true, ""},
};
static const size_t IDS_SIZE = sizeof(IDS) / sizeof(auth_t);
#endif //AUTH_H
