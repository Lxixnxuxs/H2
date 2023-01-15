//
// Created by ray on 12.01.23.
//

#ifndef H2_COMPLEXITY_TERM_HPP
#define H2_COMPLEXITY_TERM_HPP

#include <utility>
#include <vector>
#include <string>

enum ComplexityTermType {ADDITION,MULTIPLICATION,VARIABLE};

struct ComplexityTerm {
    std::vector<ComplexityTerm*> children;
    ComplexityTermType type;
    std::string name;

    ComplexityTerm(ComplexityTermType type, std::string name = ""): type(type), name(std::move(name)) {}

    void simplify(){
        for (auto e : children) e->simplify();

        if (type == ADDITION or type == MULTIPLICATION) {

            // removing redundant O(1)
            std::vector<ComplexityTerm*> new_children;
            for (auto e : children) {
                if (e->type != VARIABLE or e->name != "1") new_children.push_back(e);
            }
            if (new_children.empty()) new_children.push_back(new ComplexityTerm(VARIABLE, "1"));
            children = new_children;



            // simplify to be just the child
            if (children.size() == 1) {
                // copy all properties
                ComplexityTerm* child = children[0];
                type = child->type;
                name = child->name;
                children = child->children;
            }
        }
    }

    std::string as_string() {
        simplify();
        std::string res;

        if (type == VARIABLE) return name;

        if (type == ADDITION or type == MULTIPLICATION) {
            std::string op_symbol = (type == ADDITION) ? "+" : "*";

            for (int i = 0; i<children.size(); i++) {
                if (i!=0) res += " " + op_symbol + " ";

                if (children[i]->type != VARIABLE) res += "(";
                res += children[i]->as_string();
                if (children[i]->type != VARIABLE) res += ")";

            }

        }

        return res;
    }
};

#endif //H2_COMPLEXITY_TERM_HPP
