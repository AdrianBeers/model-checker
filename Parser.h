//
// Created by 20192920 on 25/11/2022.
//

#include "Formulae.h"
#include <string>
#include <memory>

using namespace std;

#ifndef MODEL_CHECKER_PARSER_H
#define MODEL_CHECKER_PARSER_H


class Parser {
    string I;
    int i;

    shared_ptr<Formula> parseFormula();
    shared_ptr<Formula> parseTrueLiteral();
    shared_ptr<Formula> parseFalseLiteral();
    shared_ptr<Formula> parseRecursionVariable();
    shared_ptr<Formula> parseLogicFormula();
    OperatorType parseOperator();
    OperatorType parseLogicAndOperator();
    OperatorType parseLogicOrOperator();
    shared_ptr<Formula> parseMuFormula();
    shared_ptr<Formula> parseNuFormula();
    shared_ptr<Formula> parseDiamondFormula();
    shared_ptr<Formula> parseBoxFormula();
    string parseActionName();

    void expect(const string& e);
    void skipWhiteSpace();
    void requireWhiteSpace();
public:
    shared_ptr<Formula> parse(string input);
};


#endif //MODEL_CHECKER_PARSER_H
