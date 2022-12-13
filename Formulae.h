//
// Created by 20192920 on 25/11/2022.
//

#include <string>
#include <memory>

using namespace std;

#ifndef MODEL_CHECKER_FORMULAE_H
#define MODEL_CHECKER_FORMULAE_H


enum class FormulaType {
    trueLiteral,
    falseLiteral,
    recursionVariable,
    logicFormula,
    muFormula,
    nuFormula,
    diamondFormula,
    boxFormula,
};

enum class OperatorType {
    andOperator,
    orOperator
};

class Formula {
public:
    FormulaType type;
    shared_ptr<Formula> f;
    shared_ptr<Formula> g;
    shared_ptr<Formula> r;
    OperatorType o;
    string a;
    char n;

    void pprint();

    int ND();

    int maxMuNu(bool isMu);
    int AD();

    int dAD();
};

class TrueLiteral: public Formula {
public:
    TrueLiteral();
};

class FalseLiteral: public Formula {
public:
    FalseLiteral();
};

class RecursionVariable: public Formula {
public:
    explicit RecursionVariable(char n);
};

class LogicFormula: public Formula {
public:
    LogicFormula(shared_ptr<Formula> f, shared_ptr<Formula> g, OperatorType o);
};

class MuFormula: public Formula {
public:
    MuFormula(shared_ptr<Formula> r, shared_ptr<Formula> f);
};

class NuFormula: public Formula {
public:
    NuFormula(shared_ptr<Formula> r, shared_ptr<Formula> f);
};

class DiamondFormula: public Formula {
public:
    DiamondFormula(string a, shared_ptr<Formula> f);
};

class BoxFormula: public Formula {
public:
    BoxFormula(string a, shared_ptr<Formula> f);
};


#endif //MODEL_CHECKER_FORMULAE_H
