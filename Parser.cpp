//
// Created by 20192920 on 25/11/2022.
//

#include "Parser.h"
#include "Formulae.h"
#include <memory>
#include <utility>
#include <stdexcept>

using namespace std;

bool trueLiteralFirst(char c) { return c == 't'; }

bool falseLiteralFirst(char c) { return c == 'f'; }

bool recursionVariableFirst(char c) { return c >= 'A' && c <= 'Z'; }

bool logicFormulaFirst(char c) { return c == '('; }

bool muFormulaFirst(char c) { return c == 'm'; }

bool nuFormulaFirst(char c) { return c == 'n'; }

bool diamondFormulaFirst(char c) { return c == '<'; }

bool boxFormulaFirst(char c) { return c == '['; }

bool formulaFirst(char c) {
    return trueLiteralFirst(c) ||
        falseLiteralFirst(c) ||
        recursionVariableFirst(c) ||
        logicFormulaFirst(c) ||
        muFormulaFirst(c) ||
        nuFormulaFirst(c) ||
        diamondFormulaFirst(c) ||
        boxFormulaFirst(c);
}

bool andOperatorFirst(char c) { return c == '&'; }

bool orOperatorFirst(char c) { return c == '|'; }

bool operatorFirst(char c) { return (andOperatorFirst(c) || orOperatorFirst(c)); }

bool actionNameFirst(char c) { return (c >= 'a' && c <= 'z'); }

bool actionNameNotFirst(char c) { return (actionNameFirst(c) || (c >= '0' && c <= '9') || c == '_'); }


shared_ptr<Formula> Parser::parse(string input) {
    this->I = std::move(input);
    this->i = 0;

    skipWhiteSpace();

    if (formulaFirst(I[i])) {
        shared_ptr<Formula> f = parseFormula();

        if (i < I.length()) {
            throw invalid_argument("Parse Exception in parse");
        } else {
            return f;
        }
    } else {
        throw invalid_argument("Parse Exception in parse");
    }
}

shared_ptr<Formula> Parser::parseFormula() {
    if (trueLiteralFirst(I[i])) {
        return parseTrueLiteral();
    } else if (falseLiteralFirst(I[i])) {
        return parseFalseLiteral();
    } else if (recursionVariableFirst(I[i])) {
        return parseRecursionVariable();
    } else if (logicFormulaFirst(I[i])) {
        return parseLogicFormula();
    } else if (muFormulaFirst(I[i])) {
        return parseMuFormula();
    } else if (nuFormulaFirst(I[i])) {
        return parseNuFormula();
    } else if (diamondFormulaFirst(I[i])) {
        return parseDiamondFormula();
    } else if (boxFormulaFirst(I[i])) {
        return parseBoxFormula();
    } else {
        throw invalid_argument("Parse Exception in parseFormula");
    }
}

shared_ptr<Formula> Parser::parseTrueLiteral() {
    expect("true");
    skipWhiteSpace();
    shared_ptr<Formula> t = make_shared<TrueLiteral>();
    return t;
}

shared_ptr<Formula> Parser::parseFalseLiteral() {
    expect("false");
    skipWhiteSpace();
    shared_ptr<Formula> f = make_shared<FalseLiteral>();
    return f;
}

shared_ptr<Formula> Parser::parseRecursionVariable() {
    char n = I[i];
    i++;
    skipWhiteSpace();
    shared_ptr<Formula> r = make_shared<RecursionVariable>(n);
    return r;
}

shared_ptr<Formula> Parser::parseLogicFormula() {
    expect("(");
    skipWhiteSpace();

    shared_ptr<Formula> f;
    shared_ptr<Formula> g;
    OperatorType o;

    if (formulaFirst(I[i])) {
        f = parseFormula();
    } else {
        throw invalid_argument("Parse Exception in parseLogicFormula: f");
    }

    if (operatorFirst(I[i])) {
        o = parseOperator();
    } else {
        throw invalid_argument("Parse Exception in parseLogicFormula: o");
    }

    if (formulaFirst(I[i])) {
        g = parseFormula();
    } else {
        throw invalid_argument("Parse Exception in parseLogicFormula: g");
    }

    expect(")");
    skipWhiteSpace();

    shared_ptr<Formula> l = make_shared<LogicFormula>(f, g, o);
    return l;
}

OperatorType Parser::parseOperator() {
    if (andOperatorFirst(I[i])) {
        return parseLogicAndOperator();
    } else if (orOperatorFirst(I[i])) {
        return parseLogicOrOperator();
    } else {
        throw invalid_argument("Parse Exception in parseOperator");
    }
}

OperatorType Parser::parseLogicAndOperator() {
    expect("&&");
    skipWhiteSpace();
    return OperatorType::andOperator;
}

OperatorType Parser::parseLogicOrOperator() {
    expect("||");
    skipWhiteSpace();
    return OperatorType::orOperator;
}

shared_ptr<Formula> Parser::parseMuFormula() {
    expect("mu");
    requireWhiteSpace();

    shared_ptr<Formula> r;
    shared_ptr<Formula> f;

    if (recursionVariableFirst(I[i])) {
        r = parseRecursionVariable();
    } else {
        throw invalid_argument("Parse Exception in parseMuFormula: r");
    }

    expect(".");
    skipWhiteSpace();

    if (formulaFirst(I[i])) {
        f = parseFormula();
    } else {
        throw invalid_argument("Parse Exception in parseMuFormula: f");
    }

    shared_ptr<Formula> m = make_shared<MuFormula>(r, f);
    return m;
}

shared_ptr<Formula> Parser::parseNuFormula() {
    expect("nu");
    requireWhiteSpace();

    shared_ptr<Formula> r;
    shared_ptr<Formula> f;

    if (recursionVariableFirst(I[i])) {
        r = parseRecursionVariable();
    } else {
        throw invalid_argument("Parse Exception in parseNuFormula: r");
    }

    expect(".");
    skipWhiteSpace();

    if (formulaFirst(I[i])) {
        f = parseFormula();
    } else {
        throw invalid_argument("Parse Exception in parseNuFormula: f");
    }

    shared_ptr<Formula> n = make_shared<NuFormula>(r, f);
    return n;
}

shared_ptr<Formula> Parser::parseDiamondFormula() {
    expect("<");
    skipWhiteSpace();

    string a;
    shared_ptr<Formula> f;

    if (actionNameFirst(I[i])) {
        a = parseActionName();
    } else {
        throw invalid_argument("Parse Exception in parseDiamondFormula: a");
    }

    expect(">");
    skipWhiteSpace();

    if (formulaFirst(I[i])) {
        f = parseFormula();
    } else {
        throw invalid_argument("Parse Exception in parseDiamondFormula: f");
    }

    shared_ptr<Formula> d = make_shared<DiamondFormula>(a, f);
    return d;
}

shared_ptr<Formula> Parser::parseBoxFormula() {
    expect("[");
    skipWhiteSpace();

    string a;
    shared_ptr<Formula> f;

    if (actionNameFirst(I[i])) {
        a = parseActionName();
    } else {
        throw invalid_argument("Parse Exception in parseBoxFormula: a");
    }

    expect("]");
    skipWhiteSpace();

    if (formulaFirst(I[i])) {
        f = parseFormula();
    } else {
        throw invalid_argument("Parse Exception in parseBoxFormula: f");
    }

    shared_ptr<Formula> b = make_shared<BoxFormula>(a, f);
    return b;
}

string Parser::parseActionName() {
    string n;

    if (actionNameFirst(I[i])) {
        n.push_back(I[i]);
        i++;
    } else {
        throw invalid_argument("Parse Exception in parseActionName");
    }

    while (actionNameNotFirst(I[i])) {
        n.push_back(I[i]);
        i++;
    }

    return n;
}

void Parser::expect(const string& e) {
    if (I.substr(i, e.length()) != e) {
        throw invalid_argument("Parse Exception in expect");
    } else {
        i += e.length();
    }
}

void Parser::skipWhiteSpace() {
    while (I[i] == ' ') {
        i++;
    }
}

void Parser::requireWhiteSpace() {
    if (I[i] != ' ') {
        throw invalid_argument("Parse Exception in requireWhiteSpace");
    }

    skipWhiteSpace();
}
