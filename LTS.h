//
// Created by jens on 30-11-2022.
//

#include <cstdint>
#include <string>
#include <map>
#include <set>
#include <memory>

using namespace std;

#ifndef MODEL_CHECKER_LTS_H
#define MODEL_CHECKER_LTS_H

// Type definition for (sub)set of states in LTS
typedef set<uint32_t> sset;

// Type definition for key of edge map: (src_state, action_label)
typedef pair<const uint32_t, const string> lts_map_key;

class LTS {
public:
    uint32_t initialState;
    uint32_t nrStates;
    uint32_t nrTransitions;

    shared_ptr<sset> states;
    map<lts_map_key, set<uint32_t>> edges;

    LTS();

    void pprint();
};


#endif //MODEL_CHECKER_LTS_H
