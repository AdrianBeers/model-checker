//
// Created by 20192920 on 25/11/2022.
//

#include "Formulae.h"
#include <iostream>
#include <utility>

using namespace std;

void Formula::pprint() {
    switch (type) {
        case FormulaType::trueLiteral:
            cout << "true";
            break;
        case FormulaType::falseLiteral:
            cout << "false";
            break;
        case FormulaType::recursionVariable:
            cout << n;
            break;
        case FormulaType::logicFormula:
            cout << "(";
            f->pprint();

            if (o == OperatorType::andOperator) {
                cout << " && ";
            } else if (o == OperatorType::orOperator) {
                cout << " || ";
            }

            g->pprint();
            cout << ")";
            break;
        case FormulaType::muFormula:
            cout << "mu ";
            r->pprint();
            cout << ".";
            f->pprint();
            break;
        case FormulaType::nuFormula:
            cout << "nu ";
            r->pprint();
            cout << ".";
            f->pprint();
            break;
        case FormulaType::diamondFormula:
            cout << "<" << a << ">";
            f->pprint();
            break;
        case FormulaType::boxFormula:
            cout << "[" << a << "]";
            f->pprint();
            break;
    }
}

TrueLiteral::TrueLiteral() : Formula() {
    this->type = FormulaType::trueLiteral;
}

FalseLiteral::FalseLiteral() : Formula() {
    this->type = FormulaType::falseLiteral;
}

RecursionVariable::RecursionVariable(char n) : Formula() {
    this->type = FormulaType::recursionVariable;
    this->n = n;
}

LogicFormula::LogicFormula(shared_ptr<Formula> f, shared_ptr<Formula> g, OperatorType o) : Formula() {
    this->type = FormulaType::logicFormula;
    this->f = std::move(f);
    this->g = std::move(g);
    this->o = o;
}

MuFormula::MuFormula(shared_ptr<Formula> r, shared_ptr<Formula> f) : Formula() {
    this->type = FormulaType::muFormula;
    this->r = std::move(r);
    this->f = std::move(f);
}

NuFormula::NuFormula(shared_ptr<Formula> r, shared_ptr<Formula> f) : Formula() {
    this->type = FormulaType::nuFormula;
    this->r = std::move(r);
    this->f = std::move(f);
}

DiamondFormula::DiamondFormula(string a, shared_ptr<Formula> f) : Formula() {
    this->type = FormulaType::diamondFormula;
    this->a = std::move(a);
    this->f = std::move(f);
}

BoxFormula::BoxFormula(string a, shared_ptr<Formula> f) : Formula() {
    this->type = FormulaType::boxFormula;
    this->a = std::move(a);
    this->f = std::move(f);
}
