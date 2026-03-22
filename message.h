#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>

// Message types
#define MSG_HELLO       1
#define MSG_SEARCH_REQ  2
#define MSG_SEARCH_RESP 3
#define MSG_DOWNLOAD    4
#define MSG_FILE_DATA   5

// HELLO message payload (sent immediately after connection)
struct HelloMsg {
    uint32_t node_id;
    uint16_t port;
    uint16_t version;
};

#endif 