
#ifndef H2_ASTNODE_HPP
#define H2_ASTNODE_HPP

#include "../term.hpp"
#include "../code_style.hpp"

enum ASTNodeType {};

std::string get_indention(int level) {
    return std::string(level * block_indention,' ');
}

struct ASTNode {
    Term* complexity;
    bool complexity_is_custom = false;

    int block_level;

    virtual void set_block_level(int n = 0){
        block_level = n;
    };

    virtual std::string compile() = 0;

    virtual Term* calculate_complexity() = 0;

    virtual std::string to_code() = 0;


};



#endif //H2_ASTNODE_HPP
