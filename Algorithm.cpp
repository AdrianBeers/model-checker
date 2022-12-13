//
// Created by jens on 5-12-2022.
//

#include "Algorithm.h"
#include <utility>

// Type definition of contextual values of recursion variables
typedef map<char, shared_ptr<sset>> context;

/* Utility functions */

shared_ptr<sset> emptySet() {
    return make_shared<sset>();
}

shared_ptr<sset> ssetUnion(const shared_ptr<sset> s1, const shared_ptr<sset> s2) {
    shared_ptr<sset> result = make_shared<sset>();
    set_union(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(*result, result->begin()));
    return result;
}

shared_ptr<sset> ssetIntersect(const shared_ptr<sset> s1, const shared_ptr<sset> s2) {
    shared_ptr<sset> result = make_shared<sset>();
    set_intersection(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(*result, result->begin()));
    return result;
}

shared_ptr<sset> ssetDifference(const shared_ptr<sset> s1, const shared_ptr<sset> s2) {
    shared_ptr<sset> result = make_shared<sset>();
    set_difference(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(*result, result->begin()));
    return result;
}


/* Algorithm implementations */

shared_ptr<sset> naiveEval(const shared_ptr<LTS> lts, const shared_ptr<Formula> f, context &e) {
    // Return set of states of LTS that satisfies the formula f
    switch (f->type) {
        case FormulaType::trueLiteral:
            return lts->states;
        case FormulaType::falseLiteral:
            return emptySet();
        case FormulaType::recursionVariable:
            if (e.contains(f->n)) {
                return e[f->n];
            } else {
                return emptySet();
            }
        case FormulaType::logicFormula: {
            const shared_ptr<sset> evalF = naiveEval(lts, f->f, e);
            const shared_ptr<sset> evalG = naiveEval(lts, f->g, e);
            if (f->o == OperatorType::andOperator) {
                return ssetIntersect(evalF, evalG);
            } else if (f->o == OperatorType::orOperator) {
                return ssetUnion(evalF, evalG);
            } else {
                throw invalid_argument("invalid logic operator type");
            }
        }
        case FormulaType::boxFormula:
        case FormulaType::diamondFormula: {
            // Evaluate sub formula on all states
            const shared_ptr<sset> evalF = naiveEval(lts, f->f, e);

            // Compute for which states the formula holds
            shared_ptr<sset> result = emptySet();
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

            // Prepare context used in fixpoint computation
            context eNew(e);
            eNew[n] = (f->type == FormulaType::nuFormula) ? lts->states : emptySet();

            // Iterate until fixpoint has been computed
            shared_ptr<sset> old;
            do {
                old = eNew[n];
                eNew[n] = naiveEval(lts, f->f, eNew);
            } while (*old != *eNew[n]); // set-inequality comparison
            return eNew[n];
        }
        default:
            throw invalid_argument("invalid formula type");
    }
}

shared_ptr<sset> naiveSolve(const shared_ptr<LTS> lts, const shared_ptr<Formula> f) {
    context e;
    return naiveEval(lts, f, e);
}