//
// Created by ray on 12.01.23.
//

#ifndef H2_TERM_HPP
#define H2_TERM_HPP

#include <vector>
#include <string>

enum TermType {ADDITION,MULTIPLICATION,VARIABLE};

struct Term {
    std::vector<Term*> children;
    TermType type;
    std::string name;

    Term(TermType type, std::string name = ""): type(type), name(name) {}

    void simplify(){
        for (auto e : children) e->simplify();

        if (type == ADDITION or type == MULTIPLICATION) {

            // removing redundant O(1)
            std::vector<Term*> new_children;
            for (auto e : children) {
                if (e->type != VARIABLE or e->name != "1") new_children.push_back(e);
            }
            if (new_children.empty()) new_children.push_back(new Term(VARIABLE,"1"));
            children = new_children;



            // simplify to be just the child
            if (children.size() == 1) {
                // copy all properties
                Term* child = children[0];
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
            res += "(";
            for (int i = 0; i<children.size(); i++) {
                if (i!=0) res += " " + op_symbol + " ";
                res += children[i]->as_string();
            }
            res += ")";
        }

        return res;
    }
};

#endif //H2_TERM_HPP
