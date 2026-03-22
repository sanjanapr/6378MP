#include "socket_util.h"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>

using namespace std;

// Send all bytes or fail
int send_all(int fd, const void *buf, size_t len) {
    const uint8_t *ptr = (const uint8_t *)buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t sent = send(fd, ptr, remaining, 0);

        if (sent < 0) {
            perror("send_all: send() failed");
            return -1;
        }

        if (sent == 0) {
            cerr << "send_all: connection closed by peer" << endl;
            return -1;
        }

        ptr += sent;
        remaining -= sent;
    }

    return len;
}

// Receive exactly len bytes or fail
int recv_all(int fd, void *buf, size_t len) {
    uint8_t *ptr = (uint8_t *)buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t received = recv(fd, ptr, remaining, 0);

        if (received < 0) {
            perror("recv_all: recv() failed");
            return -1;
        }

        if (received == 0) {
            cerr << "recv_all: connection closed by peer" << endl;
            return -1;
        }

        ptr += received;
        remaining -= received;
    }

    return len;
}

// Send a message with header + payload
int send_message(int fd, uint16_t type, const void *payload, uint32_t len) {
    MsgHeader header;
    header.type = htons(type);  // convert to network byte order
    header.length = htonl(len);

    // Send header
    if (send_all(fd, &header, sizeof(header)) < 0) {
        return -1;
    }

    // Send payload
    if (len > 0) {
        if (send_all(fd, payload, len) < 0) {
            return -1;
        }
    }

    return 0;
}

// Receive a message with header + payload
int recv_message(int fd, uint16_t *type, void **payload, uint32_t *len) {
    MsgHeader header;

    // Receive header
    if (recv_all(fd, &header, sizeof(header)) < 0) {
        return -1;
    }

    // Convert from network byte order
    *type = ntohs(header.type);
    *len = ntohl(header.length);

    // Allocate payload buffer
    if (*len > 0) {
        *payload = malloc(*len);
        if (*payload == NULL) {
            cerr << "recv_message: malloc failed" << endl;
            return -1;
        }

        if (recv_all(fd, *payload, *len) < 0) {
            free(*payload);
            *payload = NULL;
            return -1;
        }
    } else {
        *payload = NULL;
    }

    return 0;
}
