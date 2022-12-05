//
// Created by jens on 5-12-2022.
//

#include <cstdint>
#include <map>
#include <set>
#include "LTS.h"
#include "Formulae.h"

using namespace std;

#ifndef MODEL_CHECKER_ALGORITHM_H
#define MODEL_CHECKER_ALGORITHM_H


typedef set<uint32_t> vset;
typedef map<char, shared_ptr<vset>> context;


shared_ptr<vset> naiveMuCalculusSolve(shared_ptr<LTS> lts, shared_ptr<Formula> f);


#endif //MODEL_CHECKER_ALGORITHM_H
