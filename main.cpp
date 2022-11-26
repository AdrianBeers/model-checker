#include "Parser.h"
#include "Formulae.h"
#include <iostream>
#include <string>
#include <memory>

using namespace std;

int main() {
    cout << "Enter mu-calculus formula:" << endl;

    // Example input: (([action]true && mu X.[p]X) || Y)
    string input;
    getline(cin, input);

    // Parse string to formula
    Parser p;
    shared_ptr<Formula> f = p.parse(input);

    // Print formula to string
    f->pprint();
    cout << endl;

    return 0;
}
