#ifndef CONNECTION_H
#define CONNECTION_H

#include <map>
#include <mutex>
#include "config.h"

extern std::map<int, int> neighbors;
extern std::mutex neighbor_mutex;

void handleConnection(int sock, int my_node_id, const Config& config);

#endif 