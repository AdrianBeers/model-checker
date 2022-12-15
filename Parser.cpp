//
// Created by 20192920 on 25/11/2022.
//

#include "Parser.h"
#include "Formulae.h"
#include <memory>
#include <utility>
#include <stdexcept>

using namespace std;


/// Basic parsing

bool actionNameFirst(char c) { return (c >= 'a' && c <= 'z'); }

bool actionNameNotFirst(char c) { return (actionNameFirst(c) || (c >= '0' && c <= '9') || c == '_'); }

template<class T>
void Parser<T>::expect(const string &e) {
    if (I.substr(i, e.length()) != e) {
        throw invalid_argument("Parse Exception in expect at position " + to_string(i));
    } else {
        i += e.length();
    }
}

template<class T>
void Parser<T>::skipWhiteSpace() {
    bool skipRestOfLine = false;
    while (i < I.length()) {
        // Handle comments
        if (I[i] == '%') {
            skipRestOfLine = true;
            i++;
        }

        // Skip rest of line
        if (skipRestOfLine) {
            if (I[i] == '\n') {
                i++;
                skipRestOfLine = false;
                continue;
            } else {
                i++;
                continue;
            }
        }

        // Skip whitespace
        if (I[i] == ' ' || I[i] == '\t') {
            continue;
        }

        break;
    }
}

template<class T>
void Parser<T>::requireWhiteSpace() {
    if (i >= I.length() || (I[i] != ' ' && I[i] != '\t')) {
        throw invalid_argument("Parse Exception in requireWhiteSpace");
    }

    skipWhiteSpace();
}

template<class T>
void Parser<T>::skipNewLine() {
    while (i < I.length() && (I[i] == '\n' || I[i] == '\r')) {
        i++;
    }
}

template<class T>
void Parser<T>::requireNewLine() {
    if (i >= I.length() || (I[i] != '\n' && I[i] != '\n')) {
        throw invalid_argument("Parse Exception in requireNewLine");
    }

    skipNewLine();
}

template<class T>
string Parser<T>::parseActionName() {
    string n;

    if (!actionNameFirst(I[i])) {
        throw invalid_argument("Parse Exception in parseActionName");
    }

    do {
        n.push_back(I[i]);
        i++;
    } while (actionNameNotFirst(I[i]));

    return n;
}


/// Mu-calculus parsing

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

shared_ptr<Formula> MuCalculusParser::parse(string input) {
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

shared_ptr<Formula> MuCalculusParser::parseFormula() {
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

shared_ptr<Formula> MuCalculusParser::parseTrueLiteral() {
    expect("true");
    skipWhiteSpace();
    shared_ptr<Formula> t = make_shared<TrueLiteral>();
    return t;
}

shared_ptr<Formula> MuCalculusParser::parseFalseLiteral() {
    expect("false");
    skipWhiteSpace();
    shared_ptr<Formula> f = make_shared<FalseLiteral>();
    return f;
}

shared_ptr<Formula> MuCalculusParser::parseRecursionVariable() {
    char n = I[i];
    i++;
    skipWhiteSpace();
    shared_ptr<Formula> r = make_shared<RecursionVariable>(n);
    return r;
}

shared_ptr<Formula> MuCalculusParser::parseLogicFormula() {
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

OperatorType MuCalculusParser::parseOperator() {
    if (andOperatorFirst(I[i])) {
        return parseLogicAndOperator();
    } else if (orOperatorFirst(I[i])) {
        return parseLogicOrOperator();
    } else {
        throw invalid_argument("Parse Exception in parseOperator");
    }
}

OperatorType MuCalculusParser::parseLogicAndOperator() {
    expect("&&");
    skipWhiteSpace();
    return OperatorType::andOperator;
}

OperatorType MuCalculusParser::parseLogicOrOperator() {
    expect("||");
    skipWhiteSpace();
    return OperatorType::orOperator;
}

shared_ptr<Formula> MuCalculusParser::parseMuFormula() {
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

shared_ptr<Formula> MuCalculusParser::parseNuFormula() {
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

shared_ptr<Formula> MuCalculusParser::parseDiamondFormula() {
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

shared_ptr<Formula> MuCalculusParser::parseBoxFormula() {
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


/// LTS parsing

bool digitFirst(char c) { return c >= '0' && c <= '9'; }

shared_ptr<LTS> LTSParser::parse(std::string input) {
    shared_ptr<LTS> lts = make_shared<LTS>();

    I = std::move(input);
    i = 0;

    skipWhiteSpace();

    // Parse header
    expect("des");
    skipWhiteSpace();
    expect("(");
    lts->initialState = parseUnsignedInt32();
    expect(",");
    parseUnsignedInt32();
    expect(",");
    lts->nrStates = parseUnsignedInt32();
    expect(")");
    skipWhiteSpace();
    requireNewLine();

    // Check validity of initial state
    if (lts->initialState >= lts->nrStates) {
        throw invalid_argument("Parse Exception in LTSParser::parse: invalid initial state");
    }

    // Parse edges
    while (i < I.length()) {
        uint32_t src, target;
        string action;

        // Parse edge
        expect("(");
        src = parseUnsignedInt32();
        expect(",\"");
        action = parseActionName();
        expect("\",");
        target = parseUnsignedInt32();
        expect(")");
        skipWhiteSpace();
        skipNewLine();

        // Check validity of states
        if (src >= lts->nrStates) {
            throw invalid_argument("Parse Exception in LTSParser::parse: invalid source state");
        }
        if (target >= lts->nrStates) {
            throw invalid_argument("Parse Exception in LTSParser::parse: invalid target state");
        }

        // Store edge in data structure
        bool newEdgeInserted = true;
        lts_map_key key = make_pair(src, action);
        if (lts->edges.contains(key)) {
            newEdgeInserted = lts->edges[key].insert(target).second;
        } else {
            lts->edges.insert({key, {target}});
        }

        // Increase number of transitions if edge was actually inserted
        if (newEdgeInserted) {
            lts->nrTransitions++;
        }
    }

    // Fill list of states
    for (uint32_t i = 0; i < lts->nrStates; i++) {
        lts->states->insert(i);
    }

    return lts;
}

uint32_t LTSParser::parseUnsignedInt32() {
    if (!digitFirst(I[i])) {
        throw invalid_argument("Parse Exception in parseUnsignedInt32");
    }

    string n;
    do {
        n.push_back(I[i]);
        i++;
    } while (digitFirst(I[i]));

    return stoul(n);
}