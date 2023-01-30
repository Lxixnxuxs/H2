
#ifndef H2_ASTNODE_HPP
#define H2_ASTNODE_HPP

#include <string>



// TODO will this lead to problems when laying out memory? Because this has a wrong idea of how the struct looks like
//struct VirtualMathTerm;

#include "../virtual_math_term.hpp"


enum ASTNodeType {};

extern std::string get_indentation(int level);

struct ASTNode {

    bool complexity_is_custom = false;
    int block_level;

    virtual void set_block_level(int n);

    virtual std::string compile() = 0;

    virtual VirtualMathTerm get_complexity() = 0;

    virtual std::string to_code() = 0;

    virtual std::string get_class() = 0;

protected:
    VirtualMathTerm complexity;
    bool complexity_already_calculated;
};



#endif //H2_ASTNODE_HPP
