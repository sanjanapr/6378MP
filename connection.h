#ifndef CONNECTION_H
#define CONNECTION_H

#include <map>
#include <mutex>

extern std::map<int, int> neighbors;
extern std::mutex neighbor_mutex;

void handleConnection(int sock);

#endif 