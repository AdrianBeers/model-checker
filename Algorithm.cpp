//
// Created by jens on 5-12-2022.
//

#include "Algorithm.h"
#include <utility>

typedef map<char, shared_ptr<vset>> context;

/* Utility functions */

shared_ptr<vset> allStates(const shared_ptr<LTS> lts) {
    shared_ptr<vset> result = make_shared<vset>();
    for (uint32_t i = 0; i < lts->nrStates; i++) { result->insert(i); }
    return result;
}

shared_ptr<vset> emptySet() {
    return make_shared<vset>();
}

shared_ptr<vset> vUnion(const shared_ptr<vset> s1, const shared_ptr<vset> s2) {
    shared_ptr<vset> result = make_shared<vset>();
    set_union(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(*result, result->begin()));
    return result;
}

shared_ptr<vset> vIntersect(const shared_ptr<vset> s1, const shared_ptr<vset> s2) {
    shared_ptr<vset> result = make_shared<vset>();
    set_intersection(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(*result, result->begin()));
    return result;
}

shared_ptr<vset> vDifference(const shared_ptr<vset> s1, const shared_ptr<vset> s2) {
    shared_ptr<vset> result = make_shared<vset>();
    set_difference(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(*result, result->begin()));
    return result;
}


/* Algorithm implementations */

shared_ptr<vset> eval(const shared_ptr<LTS> lts, const shared_ptr<Formula> f, const context &e) {
    // Return set of states that satisfies the current formula
    switch (f->type) {
        case FormulaType::trueLiteral:
            return allStates(lts);
        case FormulaType::falseLiteral:
            return emptySet();
        case FormulaType::recursionVariable:
            if (e.contains(f->n)) {
                return e.at(f->n);
            } else {
                return emptySet();
            }
        case FormulaType::logicFormula: {
            const shared_ptr<vset> evalF = eval(lts, f->f, e);
            const shared_ptr<vset> evalG = eval(lts, f->g, e);
            if (f->o == OperatorType::andOperator) {
                return vIntersect(evalF, evalG);
            } else if (f->o == OperatorType::orOperator) {
                return vUnion(evalF, evalG);
            } else {
                throw invalid_argument("invalid logic operator type");
            }
        }
        case FormulaType::boxFormula:
        case FormulaType::diamondFormula: {
            // Evaluate sub formula on all states
            const shared_ptr<vset> evalF = eval(lts, f->f, e);

            // Compute for which states the formula holds
            shared_ptr<vset> result = emptySet();
            for (uint32_t s = 0; s < lts->nrStates; s++) {
                bool holds = (f->type == FormulaType::boxFormula);

                // Iterate over all edges starting in state s with action label 'a'
                const lts_map_key key = make_pair(s, f->a);
                if (lts->edges.contains(key)) {
                    for (const uint32_t t: lts->edges[key]) {
                        bool c = evalF->contains(t);
                        if ((!c && f->type == FormulaType::boxFormula) ||
                            (c && f->type == FormulaType::diamondFormula)) {
                            holds = !holds;
                            break;
                        }
                    }
                }

                // Update result set
                if (holds) {
                    result->insert(s);
                }
            }
            return result;
        }
        case FormulaType::muFormula:
        case FormulaType::nuFormula: {
            char n = f->r->n;

            // Prepare context for iterative algorithm
            context eNew(e);
            eNew[n] = (f->type == FormulaType::nuFormula) ? allStates(lts) : emptySet();

            // Iterate until fixpoint has been computed
            shared_ptr<vset> old;
            do {
                old = eNew[n];
                eNew[n] = eval(lts, f->f, eNew);
            } while (*old != *eNew[n]);
            return eNew[n];
        }
        default:
            throw invalid_argument("invalid formula type");
    }
}

shared_ptr<vset> naiveSolve(const shared_ptr<LTS> lts, const shared_ptr<Formula> f) {
    context e;
    return eval(lts, f, e);
}