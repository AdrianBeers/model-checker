//
// Created by 20192920 on 25/11/2022.
//

#include "Formulae.h"
#include "LTS.h"
#include <string>
#include <memory>

using namespace std;

#ifndef MODEL_CHECKER_PARSER_H
#define MODEL_CHECKER_PARSER_H

template <class T>
class Parser {
protected:
    string I;
    int i;

    void expect(const string& e);
    void skipWhiteSpace();
    void requireWhiteSpace();
public:
    virtual shared_ptr<T> parse(string input) = 0;
};

class MuCalculusParser : public Parser<Formula> {
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
public:
    shared_ptr<Formula> parse(std::string input) override;
};

class LTSParser: public Parser<LTS> {
public:
    shared_ptr<LTS> parse(std::string input) override;
};

#endif //MODEL_CHECKER_PARSER_H
