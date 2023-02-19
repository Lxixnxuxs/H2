#ifndef H2_ASTROOTNODE_HPP
#define H2_ASTROOTNODE_HPP

#include <vector>
#include <string>

#include "ASTNode.hpp"
//#include "ASTFunctionNode.hpp"
struct ASTFunctionNode;

struct ASTRootNode : ASTNode {
    std::vector<std::shared_ptr<ASTNode>> funcs_and_classes;

    ASTRootNode(std::vector<std::shared_ptr<ASTNode>>& funcs_and_classes);

    std::string compile() override;

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    void set_block_level(int n) override;

    std::string get_class() override;
};

#endif //H2_ASTROOTNODE_HPP
