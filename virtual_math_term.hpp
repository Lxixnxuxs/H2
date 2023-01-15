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

enum MathTermType {ADDITION,MULTIPLICATION,EXPONENTIAL,VARIABLE,NUMBER};

struct VirtualMathTerm {
    std::vector<VirtualMathTerm> children;   // for an exponential there are only two children
    MathTermType type;
    std::string name;
    double value;
    bool o_notation = true; // ATTENTION: O-Notation like calculation per default!

    VirtualMathTerm(MathTermType type) : type(type) {}

    VirtualMathTerm(std::string name) : type(VARIABLE), name(name) {}

    VirtualMathTerm(double value) : type(NUMBER), value(value) {}

    VirtualMathTerm()=default;

    virtual void simplify() {
        if (type == NUMBER or type == VARIABLE) return;

        // first simplify lower levels
        for (auto& e: children) e.simplify();

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
        double higher_default_value;
        std::function<double(double,double)> combination_func;
        MathTermType higher_type;

        if (type == ADDITION) {
            default_value = 0;
            combination_func = [](double a, double b){return a+b;};
            higher_type = MULTIPLICATION;
            higher_default_value = 1;

        } else if (type == MULTIPLICATION) {
            default_value = 1;
            combination_func = [](double a, double b){return a*b;};
            higher_type = EXPONENTIAL;
            higher_default_value = 1;
        }



            double over_all_value = default_value;
            std::vector<VirtualMathTerm> new_children;
            std::map<std::string,double> var_to_occurances;

            for (auto e: children) {
                if (e.type == NUMBER) {
                    over_all_value = combination_func(over_all_value,e.value);
                } else if (e.type == VARIABLE) {
                    bool found = false;

                    // does this need to be initialized first?
                    for (auto p : var_to_occurances) {
                        if (p.first == e.name) found = true;
                    }
                    if (!found) var_to_occurances[e.name]=0;
                    var_to_occurances[e.name] += 1;
                } else {
                    new_children.push_back(e);
                }
            }

            // unify all Variable - Occurances into one Exponential
            for (auto p : var_to_occurances) {
                if (p.second == higher_default_value) {
                    new_children.emplace_back(p.first);
                } else {
                    auto higher = VirtualMathTerm(higher_type);
                    higher.children.emplace_back(p.first);
                    higher.children.emplace_back(p.second);
                    new_children.push_back(higher);
                }
            }
            if (over_all_value != default_value) new_children.emplace_back(over_all_value);
            children = new_children;

            /*
            if (children.empty()) {
                children.emplace_back(default_value);
            }*/

            // simplify to be just the child
            if (children.size() == 1) {
                copy_to_me(children[0]);
                return;
            }



            //if (o_notation) turn_into_o_notation();
    }

    std::string as_string() {
        simplify();
        if (o_notation) turn_into_o_notation();
        std::string res;

        if (type == NUMBER){
            if (round(value) == value) return std::to_string(round(value));
            return std::to_string(value);
        }

        if (type == VARIABLE) return name;

        if (type == ADDITION or type == MULTIPLICATION) {
            std::string op_symbol = (type == ADDITION) ? "+" : "*";

            //res += std::string(fmax(children.size()-2,0),'(');

            for (int i = 0; i < children.size(); i++) {
                if (i != 0) res += " " + op_symbol + " ";

                bool is_primitive = (children[i].type == VARIABLE or children[i].type == NUMBER);

                if (!is_primitive) res += "(";
                res += children[i].as_string();
                if (!is_primitive) res += ")";
                //if (i>=2) res += ")";

            }

        }

        return res;
    }

    // this is a kind of simplification function which will cut off constants and will provide pure O-Notation
    void turn_into_o_notation() {
        if (type == NUMBER or type == VARIABLE) { return;}

        for (auto& e : children) e.turn_into_o_notation();

        if (type == EXPONENTIAL) { return;}

        // drop constants
        std::vector<VirtualMathTerm> new_children;
        for (auto e : children) {
            if ( e.type!= NUMBER) new_children.push_back(e);
        }
        if (new_children.empty()) new_children.emplace_back(1);
        children = new_children;
    }

private:
    void copy_to_me(VirtualMathTerm other) {
        type = other.type;
        name = other.name;
        value = other.value;
        children = other.children;
    }
};




#endif //H2_VIRTUAL_MATH_TERM_HPP
