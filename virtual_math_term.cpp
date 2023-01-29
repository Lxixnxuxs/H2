//
// Created by ray on 15.01.23.
//
#include "virtual_math_term.hpp"

#include <vector>
#include <string>
#include <functional>
#include <cassert>
#include <cmath>
#include <map>





VirtualMathTerm::VirtualMathTerm(MathTermType type, bool o_notation) : type(type), o_notation(o_notation) {}

VirtualMathTerm::VirtualMathTerm(MathTermType type, std::vector<VirtualMathTerm> children, bool o_notation): type(type), children(children), o_notation(o_notation) {}

VirtualMathTerm::VirtualMathTerm(std::string name,  bool o_notation): o_notation(o_notation) {
        try {
            double nr = stoi(name);
            *this = VirtualMathTerm(nr); // is this a correct way of calling another constructor?
        } catch (...) {
            type = VARIABLE;
            this->name = name;
        }
    }

VirtualMathTerm::VirtualMathTerm(std::string name, std::vector<VirtualMathTerm> children): type(CALL), name(name), children(children) {}

VirtualMathTerm::VirtualMathTerm(double value, bool o_notation) : type(NUMBER), value(value), o_notation(o_notation) {}
VirtualMathTerm::VirtualMathTerm(int value, bool o_notation) : type(NUMBER), value(value),o_notation(o_notation) {}

VirtualMathTerm::VirtualMathTerm(): type(ADDITION), o_notation(true) {} // default type is addition


    void VirtualMathTerm::substitude_variable(const std::string& var_name, const VirtualMathTerm& replacement) {
        if (type == NUMBER) return;
        if (type == VARIABLE and var_name == name) {
            copy_to_me(replacement);
        }

        for (auto& e : children) {
            e.substitude_variable(var_name, replacement);
        }
    }

    void VirtualMathTerm::substitude_call(const std::string& var_name, const VirtualMathTerm& replacement) {
        if (type == NUMBER or type == VARIABLE) return;
        if (type == CALL and var_name == name) {
            copy_to_me(replacement);
            return;
        }

        for (auto& e : children) {
            e.substitude_call(var_name, replacement);
        }
    }

    std::string VirtualMathTerm::as_string() {
        simplify();

        std::string res;

        if (type == NUMBER){
            if (round(value) == value) return std::to_string(round(value));
            return std::to_string(value);
        }

        if (type == VARIABLE) return name;

        if (type == EXPONENTIAL) return "("+children[0].as_string() + ")^(" + children[1].as_string()+")";

        if (type == CALL) return "CALL_"+name;

        if (type == ADDITION or type == MULTIPLICATION) {
            std::string op_symbol = (type == ADDITION) ? "+" : "*";

            res += std::string(fmax(children.size()-2,0),'(');

            for (int i = 0; i < children.size(); i++) {
                if (i != 0) res += " " + op_symbol + " ";

                bool is_primitive = (children[i].type == VARIABLE or children[i].type == NUMBER);

                if (!is_primitive) res += "(";
                res += children[i].as_string();
                if (!is_primitive) res += ")";
                if (i>=1 and i<children.size()-1) res += ")";

            }

        }

        return res;
    }

    std::vector<VirtualMathTerm> VirtualMathTerm::find_calls() {

        if (type == NUMBER or type == VARIABLE) return {};

        std::vector<VirtualMathTerm> res;

        for (auto& e : children) {
            auto lower = e.find_calls();
            res.insert(res.end(), lower.begin(), lower.end());
            //if (e.type == CALL) res.push_back(e);
        }
        if (type == CALL) res.push_back(*this);
        return res;
    }

    bool VirtualMathTerm::contains_variable(std::string var_name) {

        // does not count if it only appears inside a call !!!
        if (type == NUMBER or type == CALL) return false;
        if (type == VARIABLE) return name == var_name;

        for (auto& e : children) {
            if (e.contains_variable(var_name)) return true;
        }
        return false;
    }

    bool VirtualMathTerm::operator==(const VirtualMathTerm& other) const {
        if (type != other.type) return false;

        if (type == NUMBER) return (value == other.value);
        if (type == VARIABLE) return (name == other.name);

        if (type == CALL and name != other.name) return false;

        // is Addition, Multiplication or Exponential
        if (children.size() != other.children.size()) return false;

        for (int i = 0; i<children.size(); i++) {
            if (!(children[i]==other.children[i])) return false;
        }
        return true;
    }

    bool VirtualMathTerm::operator!=(const VirtualMathTerm& other) const {
        return !operator==(other);
    }

    // method for calling simplify logic until a steady state is reached
    void VirtualMathTerm::simplify() {
        auto before = *this;
        simplify_logic();
        if (o_notation) turn_into_o_notation();

        // keep on simplifying, since term was still changing
        if (before!=*this) simplify();
    }

    void VirtualMathTerm::copy_to_me(VirtualMathTerm other) {
        type = other.type;
        name = other.name;
        value = other.value;
        children = other.children;
    }

    void VirtualMathTerm::simplify_same_nested_operation(){
        assert(type == ADDITION or type == MULTIPLICATION);

        // taking lower levels of the same operation to this level
        std::vector<VirtualMathTerm> new_children;
        for (auto e : children) {
            if (type != e.type) new_children.push_back(e);
            else {
                for (auto c : e.children) new_children.push_back(c);
            }
        }
        children = new_children;
    }

    // putting together multiple Number values. Adding the neutral value to an empty operation
    void VirtualMathTerm::simplify_multiple_numbers(){
        assert(type == ADDITION or type == MULTIPLICATION);
        double default_value;
        std::function<double(double,double)> combination_func;

        if (type == ADDITION) {
            default_value = 0;
            combination_func = [](double a, double b){return a+b;};

        } else if (type == MULTIPLICATION) {
            default_value = 1;
            combination_func = [](double a, double b){return a*b;};
        }

        // Adding the neutral value to an empty operation
        if (children.empty()) {
            children.emplace_back(default_value);
            return;
        }


        double over_all_value = default_value;
        std::vector<VirtualMathTerm> new_children;
        std::map<std::string,double> var_to_occurances;

        for (auto e: children) {
            if (e.type == NUMBER) {
                over_all_value = combination_func(over_all_value,e.value);
            } else new_children.push_back(e);
        }

        if (over_all_value != default_value) new_children.emplace_back(over_all_value);
        children = new_children;
    }

    void VirtualMathTerm::simplify_variable_occurances() {
        assert(type == ADDITION or type == MULTIPLICATION);
        double higher_default_value;
        MathTermType higher_type;

        if (type == ADDITION) {
            higher_type = MULTIPLICATION;
            higher_default_value = 1;

        } else if (type == MULTIPLICATION) {
            higher_type = EXPONENTIAL;
            higher_default_value = 1;
        }


        std::vector<VirtualMathTerm> new_children;
        std::map<std::string,double> var_to_occurances;

        for (auto e: children) {
            if (e.type == VARIABLE) {
                bool found = false;

                // does this need to be initialized first?
                for (auto p : var_to_occurances) {
                    if (p.first == e.name) found = true;
                }
                if (!found) var_to_occurances[e.name] = 0;
                var_to_occurances[e.name] += 1;
            } else {
                new_children.push_back(e);
            }
        }


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
        children = new_children;
    }

    void VirtualMathTerm::simplify_one_child() {
        if (children.size() == 1) {
            copy_to_me(children[0]);
            return;
        }
    }

    void VirtualMathTerm::simplify_logic() {
        if (type == NUMBER or type == VARIABLE) return;

        for (auto& e: children) e.simplify_logic();

        if (type == CALL) return;

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

        simplify_same_nested_operation();
        simplify_multiple_numbers();
        simplify_variable_occurances();
        simplify_one_child();
    }


    // this is a kind of simplification function which will cut off constants and will provide pure O-Notation
    void VirtualMathTerm::turn_into_o_notation() {
        if (type == VARIABLE or type == NUMBER or type == CALL) { return;}

        for (auto& e : children) e.turn_into_o_notation();

        if (type == EXPONENTIAL) { return;}

        // drop constants
        std::vector<VirtualMathTerm> new_children;
        for (auto e : children) {
            if ( e.type!= NUMBER) new_children.push_back(e);
        }
        if (new_children.empty()) new_children.emplace_back(1);
        children = new_children;

        if (new_children.size() == 1){
            copy_to_me(children[0]);
        }
    }


