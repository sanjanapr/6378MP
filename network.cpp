#include "network.h"
#include "connection.h"
#include "message.h"
#include "socket_util.h"

#include <thread>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <chrono>

using namespace std;

NetworkManager::NetworkManager(int id, const Config& cfg) {
    node_id = id;
    config = cfg;
}

// ================= SERVER =================
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

        thread t(handleConnection, client_sock);
        t.detach();
    }
}

// ================= CLIENT =================
void connectToNeighbors(int node_id, Config config) {
    for (int j = 0; j < config.numNodes; j++) {
        if (config.adj[node_id][j] == 1 && node_id < j) {

            int sock = socket(AF_INET, SOCK_STREAM, 0);

            sockaddr_in peer{};
            peer.sin_family = AF_INET;
            peer.sin_port = htons(config.nodes[j].port);
            inet_pton(AF_INET, config.nodes[j].ip.c_str(), &peer.sin_addr);

            while (connect(sock, (sockaddr*)&peer, sizeof(peer)) < 0) {
                this_thread::sleep_for(chrono::milliseconds(500));
            }

            // Send HELLO message
            HelloMsg hello;
            hello.node_id = node_id;
            hello.port = config.nodes[node_id].port;
            hello.version = 1;

            if (send_message(sock, MSG_HELLO, &hello, sizeof(hello)) < 0) {
                cerr << "Failed to send HELLO to node " << j << endl;
                close(sock);
                continue;
            }

            thread t(handleConnection, sock);
            t.detach();
        }
    }
}

// ================= START =================
void NetworkManager::start() {
    thread serverThread(startServer, node_id, config);

    sleep(1); // allow servers to start

    thread clientThread(connectToNeighbors, node_id, config);

    serverThread.join();
    clientThread.join();
} 