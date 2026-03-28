#include "connection.h"
#include "message.h"
#include "socket_util.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

using namespace std;

map<int, int> neighbors;
mutex neighbor_mutex;

void handleConnection(int sock, int my_node_id, const Config& config) {
    uint16_t msg_type;
    void *payload = NULL;
    uint32_t len;

    // Receive HELLO message first
    if (recv_message(sock, &msg_type, &payload, &len) < 0) {
        cerr << "Node " << my_node_id << ": Failed to receive HELLO" << endl;
        close(sock);
        return;
    }

    if (msg_type != MSG_HELLO || len < sizeof(HelloMsg)) {
        cerr << "Node " << my_node_id << ": Invalid HELLO message" << endl;
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

    cout << "Node " << my_node_id <<  " Connected with Node " << peer_id << endl;
    
    // Send HELLO back to confirm connection
    HelloMsg response;
    response.node_id = my_node_id;
    response.port = config.nodes[my_node_id].port;
    response.version = 1;

    if (send_message(sock, MSG_HELLO, &response, sizeof(response)) < 0) {
        cerr << "Node " << my_node_id << ": Failed to send HELLO response to Node " << peer_id << endl;
        {
            lock_guard<mutex> lock(neighbor_mutex);
            neighbors.erase(peer_id);
        }
        close(sock);
        return;
    }

    cout << "Node " << my_node_id << ": sent HELLO response to Node " << peer_id << endl;

    // Keep connection open for future messages
    while (true) {
        if (recv_message(sock, &msg_type, &payload, &len) < 0) {
            break;
        }

        cout << "Node " << my_node_id << " Received message type " << msg_type << " from Node " << peer_id << endl;

        if (payload) free(payload);
    }

    {
        lock_guard<mutex> lock(neighbor_mutex);
        neighbors.erase(peer_id);
    }

    close(sock);
} 
