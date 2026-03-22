#include "graph.h"
#include <vector>

using namespace std;

void dfs(int u, const Config& config, vector<bool>& visited) {
    visited[u] = true;

    for (int v = 0; v < config.numNodes; v++) {
        if (config.adj[u][v] && !visited[v]) {
            dfs(v, config, visited);
        }
    }
}

bool isConnected(const Config& config) {
    vector<bool> visited(config.numNodes, false);

    dfs(0, config, visited);

    for (bool v : visited) {
        if (!v) return false;
    }

    return true;
} 