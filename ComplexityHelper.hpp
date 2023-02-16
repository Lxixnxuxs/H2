//
// Created by ray on 04.02.23.
//

#ifndef H2_COMPLEXITYHELPER_HPP
#define H2_COMPLEXITYHELPER_HPP

#include "virtual_math_term.hpp"
#include <map>
#include <string>
#include <vector>
#include <functional>

struct ComplexityHelper {

    // the vectors contains: 1. the highest exponential base, 2. the highest polynomial exponent, 3.highest logarithmic exponent
    std::map<std::string, std::vector<double>> var_to_coefficients;

    // tells if the term was analyzable
    bool unknown = false;

    ComplexityHelper(VirtualMathTerm term);

    ComplexityHelper(bool unknown=false);

    ComplexityHelper operator*(const ComplexityHelper& other) const;

    ComplexityHelper operator+(const ComplexityHelper& other) const;

    bool dominates(const ComplexityHelper& other) const;

private:
    ComplexityHelper combine(const ComplexityHelper& other, std::function<double(double,double)> combine_function) const;
};

#endif //H2_COMPLEXITYHELPER_HPP
