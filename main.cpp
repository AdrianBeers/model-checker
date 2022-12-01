#include "Parser.h"
#include "Formulae.h"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

using namespace std;

bool readFileContents(const char* fileName, string &out) {
    // Open file stream
    ifstream in;
    in.open(fileName);

    // Check if file exists
    if (!in.is_open()) {
        return false;
    }

    // Read file contents
    stringstream stream;
    stream << in.rdbuf();
    out = stream.str();

    // Close input stream
    in.close();

    return true;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        cout << "Usage: model_checker <mu_calculus_file> <lts_file>" << endl;
        return 1;
    }

    // Read input files
    string formulaInput, ltsInput;
    if (!readFileContents(argv[1], formulaInput)) {
        cout << "Could not read mu calculus input file" << endl;
        return 1;
    }
    if (!readFileContents(argv[2], ltsInput)) {
        cout << "Could not read lts input file" << endl;
        return 1;
    }

    // Take first line of formula input file as mu-calculus formula input
    size_t eol = formulaInput.find('\n');
    if (eol != string::npos) {
        formulaInput = formulaInput.substr(0, eol);
    }

    // Parse mu-calculus formula
    MuCalculusParser mcp;
    shared_ptr<Formula> f = mcp.parse(formulaInput);

    // Print formula to console
    cout << "Parsed mu-calculus formula:" << endl;
    f->pprint();
    cout << endl;

    // Parse LTS
    LTSParser lp;
    shared_ptr<LTS> lts = lp.parse(ltsInput);

    // Print LTS
    cout << "Parsed LTS:" << endl;
    lts->pprint();

    return 0;
}
