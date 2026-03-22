#include "config.h"

Config loadConfig() {
    Config config;

    // TEMP: hardcoded (replace with file parsing later)
    config.numNodes = 3;

    config.adj = {
        {0,1,1},
        {1,0,1},
        {1,1,0}
    };

    config.nodes = {
        {"127.0.0.1", 5000},
        {"127.0.0.1", 5001},
        {"127.0.0.1", 5002}
    };

    return config;
} 