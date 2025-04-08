//
// Created by Thomas Abplanalp on 08.04.25.
//

#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <stdint.h>

#define PORT 8080
#define RECV_BUF_SIZE 128

// Interpreter function to be called on incoming message
// Must return the length of bytes to send from changed msg string.
// Returning 0 will send "OK\n"
typedef uint8_t (*server_interpreter_t)(char *msg, uint8_t len);

void socket_server_init(void);


void socket_server_interpreter(server_interpreter_t interpreter);

#endif //SOCKET_SERVER_H
