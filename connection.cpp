#include "connection.h"
#include "message.h"
#include "socket_util.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

using namespace std;

map<int, int> neighbors;
mutex neighbor_mutex;

void handleConnection(int sock) {
    uint16_t msg_type;
    void *payload = NULL;
    uint32_t len;

    // Receive HELLO message first
    if (recv_message(sock, &msg_type, &payload, &len) < 0) {
        cerr << "Failed to receive HELLO" << endl;
        close(sock);
        return;
    }

    if (msg_type != MSG_HELLO || len < sizeof(HelloMsg)) {
        cerr << "Invalid HELLO message" << endl;
        if (payload) free(payload);
        close(sock);
        return;
    }

    HelloMsg *hello = (HelloMsg *)payload;
    uint32_t peer_id = hello->node_id;
    free(payload);

    {
        lock_guard<mutex> lock(neighbor_mutex);
        neighbors[peer_id] = sock;
    }

    cout << "Connected with Node " << peer_id << endl;

    // Keep connection open for future messages
    while (true) {
        if (recv_message(sock, &msg_type, &payload, &len) < 0) {
            break;
        }

        cout << "Received message type " << msg_type << " from Node " << peer_id << endl;

        if (payload) free(payload);
    }

    {
        lock_guard<mutex> lock(neighbor_mutex);
        neighbors.erase(peer_id);
    }

    close(sock);
} 
