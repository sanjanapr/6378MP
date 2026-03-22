#ifndef SOCKET_UTIL_H
#define SOCKET_UTIL_H

#include <cstdint>
#include <cstring>

// Message header with length prefix
struct MsgHeader {
    uint16_t type;
    uint32_t length;
};

// Send all bytes or fail
// Returns: number of bytes sent, or -1 on error
int send_all(int fd, const void *buf, size_t len);

// Receive exactly len bytes or fail
// Returns: number of bytes received, or -1 on error/connection close
int recv_all(int fd, void *buf, size_t len);

// Send a message with header + payload
// Returns: 0 on success, -1 on error
int send_message(int fd, uint16_t type, const void *payload, uint32_t len);

// Receive a message with header + payload
// Caller must free *payload
// Returns: 0 on success, -1 on error
int recv_message(int fd, uint16_t *type, void **payload, uint32_t *len);

#endif
