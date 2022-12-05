//
// Created by jens on 5-12-2022.
//

#include "Algorithm.h"
#include <utility>

/* Utility functions */

shared_ptr<vset> allStates(shared_ptr<LTS> lts) {
    vset s;
    for (uint32_t i = 0; i < lts->nrStates; i++) { s.insert(i); }
    return make_shared<vset>(s);
}

shared_ptr<vset> emptySet() {
    vset s;
    return make_shared<vset>(s);
}

shared_ptr<vset> vUnion(shared_ptr<vset> s1, shared_ptr<vset> s2) {
    vset s;
    set_union(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(s, s.begin()));
    return make_shared<vset>(s);
}

shared_ptr<vset> vIntersect(shared_ptr<vset> s1, shared_ptr<vset> s2) {
    vset s;
    set_intersection(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(s, s.begin()));
    return make_shared<vset>(s);
}

shared_ptr<vset> vDifference(shared_ptr<vset> s1, shared_ptr<vset> s2) {
    vset s;
    set_difference(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(s, s.begin()));
    return make_shared<vset>(s);
}


/* Algorithm implementations */

shared_ptr<vset> eval(shared_ptr<LTS> lts, shared_ptr<Formula> f, shared_ptr<context> e) {
    // Return set of states that satisfies the current formula
    switch (f->type) {
        case FormulaType::trueLiteral:
            return allStates(std::move(lts));
        case FormulaType::falseLiteral:
            return emptySet();
        case FormulaType::recursionVariable:
            if (e->contains(f->n)) {
                return e->at(f->n);
            } else {
                return emptySet();
            }
        case FormulaType::logicFormula: {
            shared_ptr<vset> evalF = eval(lts, f->f, e),
                    evalG = eval(lts, f->g, e);
            if (f->o == OperatorType::andOperator) {
                return vIntersect(evalF, evalG);
            } else if (f->o == OperatorType::orOperator) {
                return vUnion(evalF, evalG);
            } else {
                throw invalid_argument("invalid logic operator type");
            }
        }
        case FormulaType::boxFormula: {
            shared_ptr<vset> evalF = eval(lts, f->f, e);
            
            break;
        }
        default:
            throw invalid_argument("invalid formula type");
    }
}

shared_ptr<vset> naiveMuCalculusSolve(shared_ptr<LTS> lts, shared_ptr<Formula> f) {
    context e;
    return eval(std::move(lts), std::move(f), make_shared<context>(e));
}