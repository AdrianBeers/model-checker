//
// Created by 20192920 on 25/11/2022.
//

#include "Formulae.h"

#include <utility>

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

LogicFormula::LogicFormula(Formula f, Formula g, OperatorType o) : Formula() {
    this->type = FormulaType::logicFormula;
    this->f = f;
    this->g = g;
    this->o = o;
}

MuFormula::MuFormula(RecursionVariable r, Formula f) : Formula(), r(0) {
    this->type = FormulaType::muFormula;
    this->r = r;
    this->f = f;
}

NuFormula::NuFormula(RecursionVariable r, Formula f) : Formula(), r(0) {
    this->type = FormulaType::nuFormula;
    this->r = r;
    this->f = f;
}

DiamondFormula::DiamondFormula(string a, Formula f) : Formula() {
    this->type = FormulaType::diamondFormula;
    this->a = std::move(a);
    this->f = f;
}

BoxFormula::BoxFormula(string a, Formula f) : Formula() {
    this->type = FormulaType::boxFormula;
    this->a = std::move(a);
    this->f = f;
}
