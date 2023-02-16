//
// Created by ray on 29.01.23.
//

#include "recursive_analysis.hpp"
#include <cassert>
#include <cmath>



template <typename T>
bool is_in(const T& v, std::initializer_list<T> lst)
{
    return std::find(std::begin(lst), std::end(lst), v) != std::end(lst);
}


//std::string get_param(int i) {return "param_"+std::to_string(i);}
//int extract_param_nr(std::string param) {return std::stoi(std::string(param.at(6),1));} // only working if param nr has only one digit

// checks whether the term has the form var * number (where * can be any operation)
bool is_simple(MathTermType operation, VirtualMathTerm term, std::string var_name) {
    auto is_this_variable = [](VirtualMathTerm term, std::string var_name){return term.type == VARIABLE and
                                                                                 var_name == term.name;};

    if (is_this_variable(term, var_name)) return true;

    if (term.type != operation or term.children.size() != 2) return false;
    bool one_is_variable = is_this_variable(term.children[0], var_name) or is_this_variable(term.children[1], var_name);
    bool one_is_number = term.children[0].type == NUMBER or term.children[1].type == NUMBER;
    return  one_is_variable and one_is_number;
}

bool is_simple_linear(VirtualMathTerm term, std::string var_name) {
    return is_simple(ADDITION, term, var_name);
}

double get_lin_offset(VirtualMathTerm term) {
    if (term.type == VARIABLE) return 0;
    if (term.type == ADDITION) {
        assert(term.children.size() == 2);
        if (term.children[0].type == VARIABLE) return term.children[0].value;
        return term.children[1].value;
    }
    assert(false);
}

bool has_simple_direction(Condition region) {
    if (region.math_children.size() != 2) return false;
    if (!is_in(region.type,{LESS_EQUAL,LESS,GREATER_EQUAL, GREATER})) return false;
    bool has_variable = region.math_children[0].type == VARIABLE or region.math_children[1].type == VARIABLE;
    bool has_number = region.math_children[0].type == NUMBER or region.math_children[1].type == NUMBER;
    return has_variable and has_number;
}

bool simple_direction_and_includes_zero_point(Condition region) {
    if (!has_simple_direction(region)) return false;

    if (region.type == LESS and region.math_children[1].value > 0) return true;
    if (region.type == LESS_EQUAL and region.math_children[1].value >= 0) return true;
    if (region.type == GREATER and region.math_children[1].value < 0) return true;
    if (region.type == GREATER_EQUAL and region.math_children[1].value <= 0) return true;
    return false;
}

// for now treat simple one-variable cases
// assuming only one variable on one side and only a number on the other
// the sign in between must be < <= >= >  but NOT == !=
std::vector<double> get_base_case_direction(LogicTerm base_case, std::vector<std::string> var_names) {
    std::vector<double> res;

    int var_index = (base_case.math_children[0].type == VARIABLE) ? 0 : 1;
    auto var_term = base_case.math_children[var_index];

    int index = std::distance(var_names.begin(), std::find(var_names.begin(), var_names.end(), var_term.name));//extract_param_nr(var_term.name);
    res.insert(res.end(),fmax(index,0),0);
    int value = (base_case.type == LESS or base_case.type == LESS_EQUAL)? -1 : 1;


    // take side of equation into account
    if (var_term == 1) value *= -1;
    res.push_back(value);

    res.insert(res.end(),var_names.size()-res.size(),0);

    assert(res.size() == var_names.size());
    return res;
}

std::string get_base_case_variable(LogicTerm region){
    return (region.math_children[0].type == VARIABLE) ? region.math_children[0].name : region.math_children[1].name;
}

double scalar_product(std::vector<double> a, std::vector<double> b) {
    double res = 0;
    for (int i = 0; i<a.size(); i++) {
        res += a[i]*b[i];
    }
    return res;
}


bool is_simple_multiplicative(VirtualMathTerm term, std::string var_name) {
    return is_simple(MULTIPLICATION, term, var_name);
}

static const std::pair<bool, VirtualMathTerm> O_INFINITY = {true,{"INFINITY"}};
static const std::pair<bool, VirtualMathTerm> SURRENDERED = {false,{"SURRENDERED"}};


// https://de.wikipedia.org/wiki/Master-Theorem
std::pair<bool, Complexity> masters_theorem(int a, int b, variable_name var, Complexity f) {
    f.simplify();
    VirtualMathTerm comparison = {EXPONENTIAL,{VirtualMathTerm(var),VirtualMathTerm(LOGARITHM,{b,a})}};
    comparison.simplify();

    if (comparison.grows_faster_equal(f)) {
        if (f.grows_faster_equal(comparison))
            // case 2
            return {true, {MULTIPLICATION, {VirtualMathTerm(EXPONENTIAL,{VirtualMathTerm(var),VirtualMathTerm(LOGARITHM,{b,a})}), VirtualMathTerm(LOGARITHM, {10,var}) }}  };
        else
            // case 1
            return {true, {EXPONENTIAL,{VirtualMathTerm(var),VirtualMathTerm(LOGARITHM,{b,a})}}};
    }

    // case 3
    return {true, f};
}



// returns: (successful, o-notation)
// assumes everything is in terms of the parameters
// ATTENTION it is assumed, that only one recursive branch exists
std::pair<bool, Complexity> analyze_execution_paths(std::string func_name, std::vector<std::string> var_names, std::vector<std::pair<Condition, Complexity>> base_cases,
                                                    std::vector<recursive_execution> recursive_executions) {

    // throw recursive calls out of each complexity (because they will be resolved now)
    std::for_each(recursive_executions.begin(),recursive_executions.end(),
                  [&](auto& p){std::get<1>(p).substitude_call(func_name,{1});});

    // simplify all conditions
    std::for_each(base_cases.begin(),base_cases.end(),
                  [](auto& p){p.first.simplify();});
    std::for_each(recursive_executions.begin(),recursive_executions.end(),
                  [](auto& p){std::get<0>(p).simplify();});


    // throw away cases which cannot occur
    base_cases.erase(std::remove_if(base_cases.begin(),base_cases.end(),
                                    [&](std::pair<Condition, Complexity> p){
                                        p.first.simplify();
                                        return p.first.type == FALSE;
                                    }),base_cases.end());

    recursive_executions.erase(std::remove_if(recursive_executions.begin(), recursive_executions.end(),
                                              [&](recursive_execution p){
                                                  std::get<0>(p).simplify();
                                                  return (std::get<0>(p).type == FALSE);
                                              }),recursive_executions.end());


    // if there are no recursion cases, execute a closed analysis
    if (recursive_executions.empty()) {
        VirtualMathTerm total_complexity = {ADDITION};
        std::for_each(base_cases.begin(), base_cases.end(),
                      [&](auto b) {
                          total_complexity.children.push_back(b.second);
                      });
        total_complexity.simplify();
        return {true, total_complexity};
    }

    // without base case, the recursion goes forever
    if (base_cases.empty()) return O_INFINITY;

    // do not allow multiple base cases, because not able to find out in which the recursion will terminate
    // TODO find way to do this
    if (base_cases.size() > 1) return SURRENDERED;

    // cannot handle multiple recursion cases
    if (recursive_executions.size() > 1) return SURRENDERED;
    recursive_execution recursiveExecution = recursive_executions[0];
    std::vector<Call> recursive_calls = std::get<2>(recursiveExecution);


    //check that all possible input values are covered
    LogicTerm all_areas = LogicTerm(L_AND);
    for (auto& e : base_cases) {
        all_areas.logic_children.push_back(e.first);
    }
    all_areas.logic_children.push_back(std::get<0>(recursiveExecution));
    all_areas.simplify();
    // TODO: make simplification in logic term better in order to be able to use this assert
    //assert(all_areas.type == TRUE);


    // interlude: base case has one variable and there is only one recursive call
    auto base_case = base_cases[0];
    bool only_one_call = recursive_calls.size() == 1;
    bool base_case_only_one_var = !base_case.first.is_combination();
    bool simple_base_case_shape = has_simple_direction(base_case.first);

    if (only_one_call and base_case_only_one_var and simple_base_case_shape) {
        auto call_ = recursive_calls[0];

        auto bc_dir = get_base_case_direction(base_case.first,var_names);
        auto bc_var = get_base_case_variable(base_case.first);
        int index = std::distance(var_names.begin(),std::find(var_names.begin(), var_names.end(),bc_var));
        auto bc_value = bc_dir[index];

        // linear addition case
        if (is_simple_linear(call_.children[index], bc_var)) {
            auto direction_value = get_lin_offset(call_.children[index]);
            if (direction_value * bc_value > 0) {
                // ready to return early
                VirtualMathTerm res;
                res.children.push_back({base_case.second});
                res.children.push_back({MULTIPLICATION, {{bc_var},std::get<1>(recursiveExecution)}});
                return {true, res};
            }
        }

        // multiplicative case
        if (is_simple_multiplicative(call_.children[index], bc_var) and simple_direction_and_includes_zero_point(base_case.first)) {
            // ready to return early
            VirtualMathTerm res;
            res.children.push_back({base_case.second});
            //res.children.push_back({MULTIPLICATION, { {LOGARITHM,{10,bc_var}},std::get<1>(recursiveExecution)}});

            return {true, VirtualMathTerm(ADDITION,{base_case.second, masters_theorem(1,2,bc_var,std::get<1>(recursiveExecution)).second}) };//{true, res};
        }

    }



    // check if base cases can be handled. Find "directions" of simple base cases
    std::vector<std::vector<double>> base_case_directions;
    for (auto& base_case : base_cases) {
        if (base_case.first.is_combination()) return SURRENDERED;
        if (!has_simple_direction(base_case.first)) return SURRENDERED;
        base_case_directions.push_back(get_base_case_direction(base_case.first, var_names));
    }



    // check if recursion can be handled (e.g. if it behaves in a linear way). Find linear direction in which the recursion goes
    std::vector<std::vector<double>> recursion_directions;
    for (auto& call : recursive_calls) {
        assert(call.type == CALL);

        std::vector<double> direction;
        for (int i = 0; i < call.children.size(); i++) {
            if (!is_simple_linear(call.children[i], var_names[i])) return SURRENDERED;
            direction.push_back(get_lin_offset(call.children[i]));
        }
        recursion_directions.push_back(direction);
    }

    // check if all the recursive chains will run into a base case. Store into which
    std::vector<std::pair<Condition, Complexity>> hit_base_cases;
    for (const auto& recursion_direction : recursion_directions) {
        bool will_terminate = false;
        for (int i = 0; i<base_cases.size(); i++) {
            auto base_case_direction = base_case_directions[i];
            if (scalar_product(recursion_direction, base_case_direction) > 0) {

                // TODO: which base case will be hit first? Not clear with multiple base cases
                hit_base_cases.push_back(base_cases[i]);
                will_terminate = true;
                break;
            }
        }
        if (!will_terminate) return O_INFINITY;
    }

    // provide an according complexity

    Complexity recursion_depth = {get_base_case_variable(hit_base_cases[0].first)};
    Complexity res = {ADDITION};
    res.children.push_back(hit_base_cases[0].second); // bc

    if (recursive_calls.size() == 1){
        res.children.push_back({MULTIPLICATION, {recursion_depth,std::get<1>(recursiveExecution)}});  // recursive iterations
    }

    Complexity tree_size = {EXPONENTIAL, {(int)(recursive_calls.size()),recursion_depth}};

    if (recursive_calls.size() >= 2) {
        res.children.push_back({MULTIPLICATION, {tree_size,std::get<1>(recursiveExecution)}});
    }
    res.simplify();
    return {true,res};

}



// call-helper

std::pair<bool, Complexity> analyze_execution_from_all_paths(std::string func_name, std::vector<std::string> var_names, std::vector<ExecutionPath> all_paths) {
    std::vector<std::pair<LogicTerm, VirtualMathTerm>> base_cases;
    std::vector<std::tuple<LogicTerm, VirtualMathTerm, std::vector<VirtualMathTerm>>> recursive_executions;
    for (auto &e: all_paths) {
        auto calls = e.total_complexity.find_calls();
        if (calls.empty()) {
            base_cases.emplace_back(e.condition, e.total_complexity);
        } else {
            recursive_executions.emplace_back(e.condition, e.total_complexity, calls);
        }
    }

    // delete recursive calls from the complexity of the recursive branches, because they will be resolved now

    for (auto& e: recursive_executions) {
        VirtualMathTerm& complexity = std::get<1>(e);
        complexity.substitude_call(func_name,{0});

    }

    return analyze_execution_paths(func_name, var_names, base_cases, recursive_executions);
}
