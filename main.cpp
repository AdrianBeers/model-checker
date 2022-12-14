#include "Parser.h"
#include "Formulae.h"
#include "Algorithm.h"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>

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
//    MuCalculusParser parser;
//
//    shared_ptr<Formula> a = parser.parse("(mu A. nu B. (A || B) && mu C. mu D. (C && (mu E.true || E)))");
//    a->pprint();
//    cout << endl;
//
//    shared_ptr<Formula> b = parser.parse("(mu A. nu B. (A || B) && mu C. nu D. (C && (mu E.true || E)))");
//    b->pprint();
//    cout << endl;
//
//    cout << "a: " << a->ND() << ", b: " << b->ND() << endl;
//    cout << "a: " << a->AD() << ", b: " << b->AD() << endl;
//    cout << "a: " << a->dAD() << ", b: " << b->dAD() << endl;

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
//    cout << "Parsed mu-calculus formula:" << endl;
//    f->pprint();
//    cout << endl;

    // Parse LTS
    LTSParser lp;
    shared_ptr<LTS> lts = lp.parse(ltsInput);

    // Print LTS
//    cout << "Parsed LTS:" << endl;
//    lts->pprint();

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();

    // Solve formula on LTS using naive algorithm
    shared_ptr<vset> result = naiveSolve(lts, f);

    auto t2 = high_resolution_clock::now();
    cout << "States in LTS satisfying the formula according to naive algorithm:" << endl;
    for (uint32_t s : *result) {
        cout << s << endl;
    }

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_double.count() << "ms\n";

    t1 = high_resolution_clock::now();
    // Solve formula on LTS using Emerson Lei algorithm
    shared_ptr<vset> result2 = elSolve(lts, f);
    t2 = high_resolution_clock::now();
    cout << "States in LTS satisfying the formula according to Emerson Lei:" << endl;
    for (uint32_t s : *result2) {
        cout << s << endl;
    }

    /* Getting number of milliseconds as a double. */
    ms_double = t2 - t1;

    std::cout << ms_double.count() << "ms\n";

    return 0;
}
