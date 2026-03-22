#include <iostream>
#include "config.h"
#include "graph.h"
#include "network.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./node <node_id>\n";
        return 1;
    }

    int node_id = stoi(argv[1]);

    Config config = loadConfig();

    if (!isConnected(config)) {
        cout << "Graph not connected. Exiting.\n";
        return 1;
    }

    NetworkManager net(node_id, config);
    net.start();

    return 0;
} 