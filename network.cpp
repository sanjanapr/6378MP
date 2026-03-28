#include "network.h"
#include "connection.h"
#include "message.h"
#include "socket_util.h"

#include <thread>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <chrono>

using namespace std;

NetworkManager::NetworkManager(int id, const Config& cfg) {
    node_id = id;
    config = cfg;
}

//  SERVER 
void startServer(int node_id, Config config) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config.nodes[node_id].port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);

    cout << "Node " << node_id << " listening...\n";

    while (true) {
        int client_sock = accept(server_fd, NULL, NULL);

        thread t(handleConnection, client_sock, node_id, config);
        t.detach();
    }
}

//  CLIENT HELPER 
void connectToNeighbor(int node_id, int neighbor_id, Config config) {
    cout << "Node " << node_id << ": Attempting to connect to Node " << neighbor_id
         << " at " << config.nodes[neighbor_id].ip << ":" << config.nodes[neighbor_id].port << endl;

    // Resolve hostname to IP address using getaddrinfo()
    struct addrinfo hints{}, *result = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    string port_str = to_string(config.nodes[neighbor_id].port);
    int addr_result = getaddrinfo(config.nodes[neighbor_id].ip.c_str(), port_str.c_str(), &hints, &result);

    if (addr_result != 0) {
        cerr << "Node " << node_id << ": Failed to resolve hostname for Node " << neighbor_id
             << ": " << gai_strerror(addr_result) << endl;
        return;
    }

    if (result == NULL) {
        cerr << "Node " << node_id << ": No address found for Node " << neighbor_id << endl;
        return;
    }

    // Try connecting with the resolved address
    int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock < 0) {
        cerr << "Node " << node_id << ": Failed to create socket for Node " << neighbor_id << endl;
        freeaddrinfo(result);
        return;
    }

    // Retry connection indefinitely
    int retries = 0;
    while (connect(sock, result->ai_addr, result->ai_addrlen) < 0) {
        retries++;
        if (retries % 10 == 0) { // Log every 10 attempts
            cerr << "Node " << node_id << ": Still trying to connect to Node " << neighbor_id
                 << " (attempt " << retries << ")" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    freeaddrinfo(result);

    // Send HELLO message
    HelloMsg hello;
    hello.node_id = node_id;
    hello.port = config.nodes[node_id].port;
    hello.version = 1;

    if (send_message(sock, MSG_HELLO, &hello, sizeof(hello)) < 0) {
        cerr << "Node " << node_id << ": Failed to send HELLO to Node " << neighbor_id << endl;
        close(sock);
        return;
    }

    // Receive HELLO response from peer
    uint16_t resp_type;
    void *resp_payload = NULL;
    uint32_t resp_len;
    if (recv_message(sock, &resp_type, &resp_payload, &resp_len) < 0) {
        cerr << "Node " << node_id << ": Failed to receive HELLO response from Node " << neighbor_id << endl;
        close(sock);
        return;
    }

    if (resp_payload) free(resp_payload);

    cout << "Node " << node_id << " connected with Node " << neighbor_id << endl;

    thread t(handleConnection, sock, node_id, config);
    t.detach();
}

//  CLIENT 
void connectToNeighbors(int node_id, Config config) {
    // Launch all outbound connections in parallel (concurrent, not sequential)
    for (int j = 0; j < config.numNodes; j++) {
        if (config.adj[node_id][j] == 1 && node_id < j) {
            thread t(connectToNeighbor, node_id, j, config);
            t.detach(); // Don't wait for this connection to finish
        }
    }
}

//  START 
void NetworkManager::start() {
    thread serverThread(startServer, node_id, config);

    sleep(1); // allow servers to start

    thread clientThread(connectToNeighbors, node_id, config);

    serverThread.join();
    clientThread.join();
}


