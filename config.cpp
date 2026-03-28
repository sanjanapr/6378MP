#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Config loadConfig() {
    Config config;

    // Load node registry from nodes.conf
    ifstream nodes_file("nodes.conf");
    if (!nodes_file.is_open()) {
        cerr << "Error: Could not open nodes.conf" << endl;
        exit(1);
    }

    string line;
    int node_count = 0;
    while (getline(nodes_file, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments

        stringstream ss(line);
        string node_name;
        string ip;
        int port;

        if (!(ss >> node_name >> ip >> port)) {
            cerr << "Error: Invalid line in nodes.conf: " << line << endl;
            exit(1);
        }

        config.nodes.push_back({ip, port});
        node_count++;
    }

    nodes_file.close();
    config.numNodes = node_count;

    // Load adjacency matrix from graph.adj
    ifstream adj_file("graph.adj");
    if (!adj_file.is_open()) {
        cerr << "Error: Could not open graph.adj" << endl;
        exit(1);
    }

    int row = 0;
    while (getline(adj_file, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments

        stringstream ss(line);
        vector<int> adj_row;
        int val;

        while (ss >> val) {
            adj_row.push_back(val);
        }

        if ((int)adj_row.size() != config.numNodes) {
            cerr << "Error: Adjacency matrix row " << row << " has " << adj_row.size()
                 << " elements, expected " << config.numNodes << endl;
            exit(1);
        }

        config.adj.push_back(adj_row);
        row++;
    }

    adj_file.close();

    if (row != config.numNodes) {
        cerr << "Error: Adjacency matrix has " << row << " rows, expected " << config.numNodes << endl;
        exit(1);
    }

    cout << "Loaded config: " << config.numNodes << " nodes, "
         << (config.numNodes * (config.numNodes - 1) / 2) << " potential edges" << endl;

    return config;
}

