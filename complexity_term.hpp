//
// Created by ray on 12.01.23.
//

#ifndef H2_COMPLEXITY_TERM_HPP
#define H2_COMPLEXITY_TERM_HPP

#include <utility>
#include <vector>
#include <string>
/*
#include "virtual_math_term.hpp"
//enum ComplexityTermType {ADDITION,MULTIPLICATION,VARIABLE};

struct VirtualMathTerm {

    //VirtualMathTerm(ComplexityTermType type, std::string name = ""): type(type), name(std::move(name)) {}

    void simplify() override {
        for (auto e : children) e.simplify();

        if (type == ADDITION or type == MULTIPLICATION) {

            // removing redundant O(1)
            std::vector<VirtualMathTerm> new_children;
            for (auto e : children) {
                if (e.type != VARIABLE or e.name != "1") new_children.push_back(e);
            }
            if (new_children.empty()) new_children.push_back(new VirtualMathTerm(VARIABLE, "1"));
            children = new_children;



            // simplify to be just the child
            if (children.size() == 1) {
                // copy all properties
                VirtualMathTerm* child = children[0];
                type = child.type;
                name = child.name;
                children = child.children;
            }
        }
    }
};
*/

#endif //H2_COMPLEXITY_TERM_HPP
