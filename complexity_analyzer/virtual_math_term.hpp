//
// Created by ray on 15.01.23.
//

#ifndef H2_VIRTUAL_MATH_TERM_HPP
#define H2_VIRTUAL_MATH_TERM_HPP

#include <vector>
#include <string>
#include <map>
#include <optional>

enum MathTermType {ADDITION, MULTIPLICATION, EXPONENTIAL, LOGARITHM, VARIABLE, NUMBER, CALL};

struct VirtualMathTerm {
    std::vector<VirtualMathTerm> children;   // for an exponential or logarithm there are only two children
    MathTermType type;
    std::string name;
    double value;
    bool o_notation = true;
    // ATTENTION: O-Notation like calculation per default! One O-Notation can make the whole subtree to turn into o-notation
    // TODO I should change that


    VirtualMathTerm(MathTermType type, bool o_notation = true);

    VirtualMathTerm(MathTermType type, std::vector<VirtualMathTerm> children, bool o_notation = true);

    VirtualMathTerm(std::string name,  bool o_notation = true);

    VirtualMathTerm(std::string name, std::vector<VirtualMathTerm> children);

    VirtualMathTerm(double value, bool o_notation = true);
    VirtualMathTerm(int value, bool o_notation = true);

    VirtualMathTerm();


    void substitude_variable(const std::string& var_name, const VirtualMathTerm& replacement);

    void substitude_call(const std::string& var_name, const VirtualMathTerm& replacement);

    std::string as_string();

    std::vector<VirtualMathTerm> find_calls();

    bool contains_variable(std::string var_name) const;

    bool operator==(const VirtualMathTerm& other) const;

    bool operator!=(const VirtualMathTerm& other) const;

    // method for calling simplify logic until a steady state is reached
    void simplify();


    std::optional<bool> grows_faster_equal(const VirtualMathTerm& other) const;

private:

    std::map<std::string,double> get_variable_occurances();

    void copy_to_me(VirtualMathTerm other);

    void simplify_same_nested_operation();

    // putting together multiple Number values. Adding the neutral value to an empty operation
    void simplify_multiple_numbers();

    void simplify_variable_occurances();

    void simplify_one_child();

    void simplify_addition();

    void simplify_logic();


    // this is a kind of simplification function which will cut off constants and will provide pure O-Notation
    void turn_into_o_notation();

    //void simplify_addition_o_notation();


};

extern const VirtualMathTerm UNKNOWN_VAR;


#endif //H2_VIRTUAL_MATH_TERM_HPP
