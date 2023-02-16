//
// Created by ray on 04.02.23.
//

#include "ComplexityHelper.hpp"
#include <algorithm>

ComplexityHelper::ComplexityHelper(VirtualMathTerm term) {
    if (term.type == NUMBER) {
        return;
    }
    if (term.type == VARIABLE) {
        var_to_coefficients[term.name] = {0,1,0};
        return;
    }

    if (term.type == MULTIPLICATION) {
        for (auto& child : term.children){
            *this = *this * ComplexityHelper(child);
        }
        return;
    }
    if (term.type == ADDITION) {
        for (auto& child : term.children){
            *this = *this + ComplexityHelper(child);
        }
        return;
    }


    if (term.type == EXPONENTIAL){
        if (term.children[0].type == VARIABLE and term.children[1].type == NUMBER) {
            var_to_coefficients[term.children[0].name] = {0,term.children[1].value};
            return;
        }
        if (term.children[0].type == NUMBER and term.children[1].type == VARIABLE) {
            var_to_coefficients[term.children[1].name] = {term.children[0].value,0};
            return;
        }

        unknown = true;
        return;
    }

    if (term.type == LOGARITHM){
        if (term.children[0].type == NUMBER and term.children[1].type == VARIABLE) {
            var_to_coefficients[term.children[1].name] = {0,0,1};
            return;
        }
        unknown = true;
        return;
    }

    unknown = true;
}

ComplexityHelper ComplexityHelper::operator*(const ComplexityHelper &other) const {
    return combine(other, [](double a, double b){return a+b;});
}

// TODO does this really make sense?
ComplexityHelper ComplexityHelper::operator+(const ComplexityHelper &other) const {
    return combine(other, [](double a, double b){return std::max(a,b);});
}


ComplexityHelper ComplexityHelper::combine(const ComplexityHelper &other, std::function<double(double, double)> combine_function) const {
    if (unknown or other.unknown) return ComplexityHelper(true);

    ComplexityHelper result;

    // copy coefficients from one
    for (auto& [var,coefficients] : other.var_to_coefficients){
        if (var_to_coefficients.find(var) == var_to_coefficients.end()){
            result.var_to_coefficients[var] = coefficients;
        }
    }

    // combine with coefficients from other
    for (auto& [var,coefficients] : var_to_coefficients){
        if (other.var_to_coefficients.find(var) != other.var_to_coefficients.end()){

            // attention: implicitly assumes that you meant to take the higher exponential basis
            result.var_to_coefficients[var][0] = std::max(coefficients[1], other.var_to_coefficients.at(var)[1]);

            // combine polynomial and logarithmic exponents
            for (int i = 1; i<=2; i++){
                result.var_to_coefficients[var][1] = combine_function(coefficients[1], other.var_to_coefficients.at(var)[1]);

            }


        } else {
            result.var_to_coefficients[var] = coefficients;
        }
    }

    return result;
}

bool ComplexityHelper::dominates(const ComplexityHelper &other) const {
    if (unknown or other.unknown) return false;

    // in order to dominate, every variable of the other term must be dominated

    for (auto& [var,coefficients] : other.var_to_coefficients){

        // does it have some variable that is not in this?
        if (var_to_coefficients.find(var) == var_to_coefficients.end()){
            return false;
        }

        // dominating on this variable?
        for (int i = 0; i<=2; i++){
            if (coefficients[i] < var_to_coefficients.at(var)[i]){
                break;
            }
            if (coefficients[i] > var_to_coefficients.at(var)[i]){
                return false;
            }
        }
    }
    return true;
}

ComplexityHelper::ComplexityHelper(bool unknown) {
    this->unknown = unknown;
}
