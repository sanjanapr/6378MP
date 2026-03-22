#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>

struct NodeInfo {
    std::string ip;
    int port;
};

struct Config {
    int numNodes;
    std::vector<std::vector<int>> adj;
    std::vector<NodeInfo> nodes;
};

Config loadConfig();

#endif 