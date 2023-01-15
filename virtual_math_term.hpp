//
// Created by ray on 15.01.23.
//

#ifndef H2_VIRTUAL_MATH_TERM_HPP
#define H2_VIRTUAL_MATH_TERM_HPP

#include <vector>
#include <string>
#include <functional>
#include <assert.h>
#include <math.h>
#include <map>

enum VirtualMathTermType {ADDITION,MULTIPLICATION,EXPONENTIAL,VARIABLE,NUMBER};

struct VirtualMathTerm {
    std::vector<VirtualMathTerm> children;   // for an exponential there are only two children
    VirtualMathTermType type;
    std::string name;
    double value;



    VirtualMathTerm(VirtualMathTermType type) : type(type) {}

    VirtualMathTerm(std::string name) : type(VARIABLE), name(name) {}

    VirtualMathTerm(double value) : type(NUMBER), value(value) {}

    void simplify() {
        if (type == NUMBER or type == VARIABLE) return;

        // first simplify lower levels
        for (auto e: children) e.simplify();

        if (type == EXPONENTIAL) {
            assert(children.size() == 2);
            if (children[0].type == NUMBER and children[1].type == NUMBER) {
                // transform to be only one number
                value = pow(children[0].value, children[1].value);
                type = NUMBER;
                children = {};
            }
            return;
        }

        // putting together multiple Number values
        double default_value;
        std::function<double(double,double)> combination_func;
        VirtualMathTermType higher_type;
        if (type == ADDITION) {
            default_value = 0;
            combination_func = [](double a, double b){return a+b;};
            higher_type = MULTIPLICATION;

        } else if (type == MULTIPLICATION) {
            default_value = 1;
            combination_func = [](double a, double b){return a*b;};
            higher_type = EXPONENTIAL;
        }



            double over_all_value = default_value;
            std::vector<VirtualMathTerm> new_children;
            std::map<std::string,double> var_to_occurances;

            for (auto e: children) {
                if (e.type == NUMBER) {
                    over_all_value = combination_func(over_all_value,e.value);
                } else if (e.type == VARIABLE){
                    var_to_occurances[e.name] += 1;  // does this need to be initialized first?
                } else {
                    new_children.push_back(e);
                }
            }

            // unify all Variable - Occurances into one Exponential
            for (auto p : var_to_occurances) {
                auto higher = VirtualMathTerm(higher_type);
                higher.children.emplace_back(p.first);
                higher.children.emplace_back(p.second);
                new_children.push_back(higher);
            }
            if (over_all_value != default_value) new_children.emplace_back(over_all_value);
            children = new_children;





            // simplify to be just the child
            if (children.size() == 1) {
                // copy all properties
                VirtualMathTerm child = children[0];
                type = child.type;
                name = child.name;
                children = child.children;
            }

    }

    std::string as_string() {
        simplify();
        std::string res;

        if (type == VARIABLE) return name;

        if (type == ADDITION or type == MULTIPLICATION) {
            std::string op_symbol = (type == ADDITION) ? "+" : "*";

            for (int i = 0; i < children.size(); i++) {
                if (i != 0) res += " " + op_symbol + " ";

                if (children[i]->type != VARIABLE) res += "(";
                res += children[i]->as_string();
                if (children[i]->type != VARIABLE) res += ")";

            }

        }

        return res;
    }

}

#endif //H2_VIRTUAL_MATH_TERM_HPP
