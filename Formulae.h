//
// Created by 20192920 on 25/11/2022.
//

#include <string>

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
    char n;
};

class LogicFormula: public Formula {
public:
    LogicFormula(Formula f, Formula g, OperatorType o);
    Formula f;
    Formula g;
    OperatorType o;
};

class MuFormula: public Formula {
public:
    MuFormula(RecursionVariable r, Formula f);
    RecursionVariable r;
    Formula f;
};

class NuFormula: public Formula {
public:
    NuFormula(RecursionVariable r, Formula f);
    RecursionVariable r;
    Formula f;
};

class DiamondFormula: public Formula {
public:
    DiamondFormula(string a, Formula f);
    string a;
    Formula f;
};

class BoxFormula: public Formula {
public:
    BoxFormula(string a, Formula f);
    string a;
    Formula f;
};


#endif //MODEL_CHECKER_FORMULAE_H
