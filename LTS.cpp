//
// Created by jens on 30-11-2022.
//

#include <iostream>
#include "LTS.h"

using namespace std;

void LTS::pprint() {
    cout << "des (" << initialState << "," << nrTransitions << "," << nrStates << ")" << endl;
    for (auto &edge: edges) {
        lts_map_key key = edge.first;
        for (uint32_t target : edge.second) {
            cout << "(" << key.first << ",\"" << key.second << "\"," << target << ")" << endl;
        }
    }
}