/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** myftp.c
*/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>

#include "myftp.h"
#include "server.h"

static int print_help(void)
{
    printf(""
        "USAGE: ./myftp port path\n"
        "port is the port number on which the server socket listens\n"
        "path is the path to the home directory for the Anonymous user\n"
    );
    return EXIT_SUCCESS;
}

int main(const int argc, const char *argv[])
{
    server_t server = {0};

    if (argc != 3)
        return EXIT_FAILURE_TEK;
    if (strcmp(argv[1], "-help") == 0)
        return print_help();
    errno = 0;
    server.port = strtoul(argv[1], NULL, 10);
    if (server.port == 0 || errno != 0) {
        fprintf(stderr, "myftp: invalid port number\n");
        return EXIT_FAILURE_TEK;
    }
    strcpy(server.anonymous_default_path, argv[2]);
    if (init_server(&server) == -1)
        return EXIT_FAILURE_TEK;
    printf("Listening on 0.0.0.0:%d\n", server.port);
    while (true)
        events_loop(&server);
}
