#ifndef NETWORK_H
#define NETWORK_H

#include "config.h"

class NetworkManager {
private:
    int node_id;
    Config config;

public:
    NetworkManager(int id, const Config& cfg);

    void start();
};

#endif 