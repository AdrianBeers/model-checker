//
// Created by jens on 5-12-2022.
//

#include "Algorithm.h"
#include <utility>
#include <list>
#include <map>

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

/**
 *
 * @param f
 * @param muSeen
 * @param nuSeen
 * @param a
 * @param lts
 */
void EL_initialize(const shared_ptr<Formula>& f, set<int> muSeen, set<int> nuSeen,
                                         context& a, const shared_ptr<LTS>& lts) {
    if (f->type == FormulaType::nuFormula) {
        nuSeen.insert(f->r->n);
        EL_initialize(f->f, muSeen, nuSeen, a, lts);
    } else if (f->type == FormulaType::muFormula) {
        muSeen.insert(f->r->n);
        EL_initialize(f->f, muSeen, nuSeen, a, lts);
    } else if (f->type == FormulaType::recursionVariable) {
        if (muSeen.contains(f->n)) {
            a[f->n] = emptySet();
        } else if (nuSeen.contains(f->n)) {
            a[f->n] = allStates(lts);
        }
    } else if (f->type == FormulaType::boxFormula || f->type == FormulaType::diamondFormula) {
        EL_initialize(f->f, muSeen, nuSeen, a, lts);
    } else if (f->type == FormulaType::logicFormula) {
        EL_initialize(f->f, muSeen, nuSeen, a,lts);
        EL_initialize(f->g, muSeen, nuSeen, a, lts);
    }
}

bool isOpen(const shared_ptr<Formula>& f, set<int> seen) {
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

void reset_subformulae(const shared_ptr<Formula>& f, bool mu, bool nu,
                          context& a) {
    if ((mu && f->type == FormulaType::muFormula) ||
            (nu && f->type == FormulaType::nuFormula)) {
        if (isOpen(f, {})) {
            a[f->r->n] = emptySet();
        }
        reset_subformulae(f->f, mu, nu, a);
    } else if (f->type == FormulaType::logicFormula) {
        reset_subformulae(f->f, mu, nu, a);
        reset_subformulae(f->g, mu, nu, a);
    } else if (f->type != FormulaType::trueLiteral && f->type != FormulaType::falseLiteral) {
        reset_subformulae(f->f, mu, nu, a);
    }
}

shared_ptr<vset> eval_EL(const shared_ptr<LTS>& lts, const shared_ptr<Formula>& f, context a,
                         bool muWrapped, bool nuWrapped) {
    // Return set of states that satisfies the current formula
    switch (f->type) {
        case FormulaType::trueLiteral:
            return allStates(lts);
        case FormulaType::falseLiteral:
            return emptySet();
        case FormulaType::recursionVariable:
            if (a.contains(f->n)) {
                return a.at(f->n);
            } else {
                return emptySet();
            }
        case FormulaType::logicFormula: {
            const shared_ptr<vset> evalF = eval_EL(lts, f->f, a, muWrapped, nuWrapped);
            const shared_ptr<vset> evalG = eval_EL(lts, f->g, a, muWrapped, nuWrapped);
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
            const shared_ptr<vset> evalF = eval_EL(lts, f->f, a, muWrapped, nuWrapped);

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

            // initialize array a
            EL_initialize(f, {}, {}, a, lts);

            if ((f->type == FormulaType::muFormula && nuWrapped) &&
                    (f->type == FormulaType::nuFormula && muWrapped)) {
                reset_subformulae(f->f, f->type == FormulaType::muFormula, f->type == FormulaType::nuFormula, a);
            }

            // Iterate until fixpoint has been computed
            shared_ptr<vset> old;
            do {
                old = a[n];
                a[n] = eval_EL(lts, f->f, a, f->type == FormulaType::muFormula,
                               f->type == FormulaType::nuFormula);
            } while (*old != *a[n]);
            return a[n];
        }
        default:
            throw invalid_argument("invalid formula type");
    }
}

shared_ptr<vset> elSolve(const shared_ptr<LTS>& lts, const shared_ptr<Formula>& f) {
    context a;
    return eval_EL(lts, f, a, false, false);
}