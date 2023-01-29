//
// Created by ray on 17.01.23.
//

#ifndef H2_RECURSIVE_ANALYSIS_HPP
#define H2_RECURSIVE_ANALYSIS_HPP

#include <algorithm>
#include <initializer_list>

#include "virtual_math_term.hpp"
#include "LogicTerm.hpp"
#include "ExecutionPath.hpp"



typedef VirtualMathTerm Call;
typedef VirtualMathTerm Complexity;
typedef LogicTerm Condition;
typedef std::tuple<Condition, Complexity, std::vector<Call>> recursive_execution;



template <typename T>
extern bool is_in(const T& v, std::initializer_list<T> lst);


extern bool is_simple_linear(VirtualMathTerm term, std::string var_name);

extern double get_lin_offset(VirtualMathTerm term);

extern bool has_simple_direction(Condition region);

// for now treat simple one-variable cases
// assuming only one variable on one side and only a number on the other
// the sign in between must be < <= >= >  but NOT == !=
extern std::vector<double> get_base_case_direction(LogicTerm base_case, std::vector<std::string> var_names);

extern std::string get_base_case_variable(LogicTerm region);

extern double scalar_product(std::vector<double> a, std::vector<double> b);


// returns: (successful, o-notation)
// assumes everything is in terms of the parameters
// ATTENTION it is assumed, that only one recursive branch exists
extern std::pair<bool, Complexity> analyze_execution_paths(std::string func_name, std::vector<std::string> var_names, std::vector<std::pair<Condition, Complexity>> base_cases,
                                                    std::vector<recursive_execution> recursive_executions);



// call-helper
extern std::pair<bool, Complexity> analyze_execution_from_all_paths(std::string func_name, std::vector<std::string> var_names, std::vector<ExecutionPath> all_paths);



#endif //H2_RECURSIVE_ANALYSIS_HPP
