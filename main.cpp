#include "Parser.h"
#include "Formulae.h"
#include "Algorithm.h"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstring>

using namespace std;


bool readFileContents(const char *filename, string &out) {
    // Open filename stream
    ifstream in;
    in.open(filename);

    // Check if filename exists
    if (!in.is_open()) {
        return false;
    }

    // Read filename contents
    stringstream stream;
    stream << in.rdbuf();
    out = stream.str();

    // Close input stream
    in.close();

    return true;
}

void solveUsingNaiveAlgo(const shared_ptr<LTS> &lts, const shared_ptr<Formula> &f) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::milli;

    // Solve formula on LTS using naive algorithm
    auto t1 = high_resolution_clock::now();
    shared_ptr<sset> result = naiveSolve(lts, f);
    auto t2 = high_resolution_clock::now();
    duration<double, milli> diff = t2 - t1;

    // Print results
    cout << "[Naive]: Initial state of LTS satisfies formula: ";
    cout << "[Naive]: Initial state of LTS satisfies formula: ";
    if (result->contains(lts->initialState)) {
        cout << "true";
    } else {
        cout << "false";
    }
    cout << endl;

    // Print performance statistics
    cout << "[Naive]: Number of fixpoint iterations: " << getNrFixpointIterations() << endl;
    cout << "[Naive]: Algorithm execution time: " << diff.count() << " ms" << endl;
}

void solveUsingEmAlgo(const shared_ptr<LTS> &lts, const shared_ptr<Formula> &f) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::milli;

    // Solve formula on LTS using naive algorithm
    auto t1 = high_resolution_clock::now();
    shared_ptr<sset> result = emlSolve(lts, f);
    auto t2 = high_resolution_clock::now();
    duration<double, milli> diff = t2 - t1;

    // Print results
    cout << "[Emerson-Lei]: Initial state of LTS satisfies formula: ";
    if (result->contains(lts->initialState)) {
        cout << "true";
    } else {
        cout << "false";
    }
    cout << endl;

    // Print performance statistics
    cout << "[Emerson-Lei]: Number of fixpoint iterations: " << getNrFixpointIterations() << endl;
    cout << "[Emerson-Lei]: Algorithm execution time: " << diff.count() << " ms" << endl;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Usage: model_checker <algo> <mu_calculus_file> <lts_file>" << endl;
        cout << " where <algo_id> in {naive, em, both}" << endl;
        return 1;
    }

    // Check which model-checking algorithm(s) to use
    bool useNaiveAlgorithm = false,
            useEmAlgorithm = false;
    if (strcmp(argv[1], "naive") == 0) {
        useNaiveAlgorithm = true;
    }
    if (strcmp(argv[1], "em") == 0) {
        useEmAlgorithm = true;
    }
    if (strcmp(argv[1], "both") == 0) {
        useNaiveAlgorithm = true;
        useEmAlgorithm = true;
    }
    if (!useNaiveAlgorithm && !useEmAlgorithm) {
        cerr << "Invalid argument for <algo>" << endl;
        return 1;
    }

    // Read input files
    string formulaInput, ltsInput;
    if (!readFileContents(argv[2], formulaInput)) {
        cerr << "Could not read mu calculus input file" << endl;
        return 1;
    }
    if (!readFileContents(argv[3], ltsInput)) {
        cerr << "Could not read lts input file" << endl;
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

    // Print depths of formula
    cout << "[Formula] ND = " << f->ND() << endl;
    cout << "[Formula] AD = " << f->AD() << endl;
    cout << "[Formula] dAD = " << f->dAD() << endl;

    // Parse LTS
    LTSParser lp;
    shared_ptr<LTS> lts = lp.parse(ltsInput);

    // Run model-checking algorithm
    if (useNaiveAlgorithm) {
        solveUsingNaiveAlgo(lts, f);
    }
    if (useEmAlgorithm) {
        solveUsingEmAlgo(lts, f);
    }

    return 0;
}
