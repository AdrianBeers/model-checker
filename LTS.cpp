//
// Created by jens on 30-11-2022.
//

#include <iostream>
#include "LTS.h"

using namespace std;

void LTS::pprint() {
    cout << "des (" << initialState << "," << edges.size() << "," << nrStates << ")" << endl;
    for (const auto &edge: edges) {
        lts_map_key key = edge.first;
        cout << "(" << key.first << ",\"" << key.second << "\"," << edge.second << ")" << endl;
    }
}