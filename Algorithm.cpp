//
// Created by jens on 5-12-2022.
//

#include "Algorithm.h"
#include <utility>
#include <list>
#include <iostream>
#include <map>

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


/* Naive algorithm implementation */

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

/* Emerson-Lei algorithm implementation */

void initializeEL(const shared_ptr<Formula> &f, context &a, const shared_ptr<LTS> &lts) {
    if (f->type == FormulaType::nuFormula) {
        a[f->r->n] = lts->states;
        initializeEL(f->f, a, lts);
    } else if (f->type == FormulaType::muFormula) {
        a[f->r->n] = emptySet();
        initializeEL(f->f, a, lts);
    } else if (f->type == FormulaType::boxFormula || f->type == FormulaType::diamondFormula) {
        initializeEL(f->f, a, lts);
    } else if (f->type == FormulaType::logicFormula) {
        initializeEL(f->f, a, lts);
        initializeEL(f->g, a, lts);
    }
}

bool isOpen(const shared_ptr<Formula> &f, set<char> seen) {
    if (f->type == FormulaType::nuFormula) {
        seen.insert(f->r->n);
        return isOpen(f->f, seen);
    } else if (f->type == FormulaType::muFormula) {
        seen.insert(f->r->n);
        return isOpen(f->f, seen);
    } else if (f->type == FormulaType::recursionVariable) {
        if (!seen.contains(f->n)) {
            return true;
        } else {
            return false;
        }
    } else if (f->type == FormulaType::boxFormula || f->type == FormulaType::diamondFormula) {
        return isOpen(f->f, seen);
    } else if (f->type == FormulaType::logicFormula) {
        return isOpen(f->f, seen) || isOpen(f->g, seen);
    } else {
        return false;
    }
}

void resetSubformulae(const shared_ptr<Formula> &f, bool mu, bool nu,
                      context &a, shared_ptr<LTS> lts) {
    if ((mu && f->type == FormulaType::muFormula) ||
        (nu && f->type == FormulaType::nuFormula)) {
        if (isOpen(f, {})) {
            f->type == FormulaType::muFormula ? emptySet() : lts->states;
        }
        resetSubformulae(f->f, mu, nu, a, lts);
    } else if (f->type == FormulaType::logicFormula) {
        resetSubformulae(f->f, mu, nu, a, lts);
        resetSubformulae(f->g, mu, nu, a, lts);
    } else if (f->type == FormulaType::diamondFormula || f->type == FormulaType::boxFormula
               || f->type == FormulaType::muFormula || f->type == FormulaType::nuFormula) {
        resetSubformulae(f->f, mu, nu, a, lts);
    }
}

shared_ptr<sset> emlSolve(const shared_ptr<LTS> &lts, const shared_ptr<Formula> &f, context &a,
                          bool muWrapped, bool nuWrapped) {
    // Return set of states that satisfies the current formula
    switch (f->type) {
        case FormulaType::trueLiteral:
            return lts->states;
        case FormulaType::falseLiteral:
            return emptySet();
        case FormulaType::recursionVariable:
            if (a.contains(f->n)) {
                return a.at(f->n);
            } else {
                return emptySet();
            }
        case FormulaType::logicFormula: {
            const shared_ptr<sset> evalF = emlSolve(lts, f->f, a, muWrapped, nuWrapped);
            const shared_ptr<sset> evalG = emlSolve(lts, f->g, a, muWrapped, nuWrapped);
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
            const shared_ptr<sset> evalF = emlSolve(lts, f->f, a, muWrapped, nuWrapped);

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

            if ((f->type == FormulaType::muFormula && nuWrapped) ||
                (f->type == FormulaType::nuFormula && muWrapped)) {
                resetSubformulae(f, f->type == FormulaType::muFormula, f->type == FormulaType::nuFormula, a, lts);
            }

            // Iterate until fixpoint has been computed
            shared_ptr<sset> old;
            do {
                old = a[n];
                a[n] = emlSolve(lts, f->f, a, f->type == FormulaType::muFormula,
                                f->type == FormulaType::nuFormula);
            } while (*old != *a[n]);
            return a[n];
        }
        default:
            throw invalid_argument("invalid formula type");
    }
}

shared_ptr<sset> emlSolve(const shared_ptr<LTS> &lts, const shared_ptr<Formula> &f) {
    context a;
    initializeEL(f, a, lts);
    return emlSolve(lts, f, a, false, false);
}