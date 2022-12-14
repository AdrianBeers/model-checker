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

// Solve formula f using the naive algorithm
shared_ptr<sset> naiveSolve(shared_ptr<LTS> lts, shared_ptr<Formula> f);

// Solve formula f using the Emerson-Lei algorithm
shared_ptr<sset> emlSolve(const shared_ptr<LTS>& lts, const shared_ptr<Formula>& f);

#endif //MODEL_CHECKER_ALGORITHM_H
