#ifndef H2_VARASSIGNMENTS_HPP
#define H2_VARASSIGNMENTS_HPP

#include <unordered_map>
#include <string>

using namespace std;

class VarAssignments {

    // map from symbolic name to register (later to be substituted with memory address)
    unordered_map<string, string> vars;

public:
    void put(string& name, string& reg) {
        vars[name] = reg;
    }

    string get()

};

#endif //H2_VARASSIGNMENTS_HPP
