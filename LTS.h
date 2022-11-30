//
// Created by jens on 30-11-2022.
//

#include <cstdint>
#include <string>
#include <map>

using namespace std;

#ifndef MODEL_CHECKER_LTS_H
#define MODEL_CHECKER_LTS_H

typedef pair<const uint32_t, const string> lts_map_key;

class LTS {
public:
    uint32_t initialState;
    uint32_t nrStates;
    map<const lts_map_key, const uint32_t> edges;
};


#endif //MODEL_CHECKER_LTS_H
