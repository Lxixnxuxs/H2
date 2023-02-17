#ifndef H2_ASTFUNCTIONNODE_HPP
#define H2_ASTFUNCTIONNODE_HPP


#include <vector>
#include "../global_information.hpp"
#include "../complexity_analyzer/virtual_math_term.hpp"
struct ASTStatementNode;


struct ASTFunctionNode : public ASTNode {

    std::string f_name;
    size_t f_stack_size;
    size_t arg_stackpart_size;
    std::vector<ASTStatementNode*> body;
    std::vector<std::pair<std::string,std::string>> argument_list;  // name to type
    std::string return_type;

    // not always will it be possible to figure out the runtime. Therefore the logic has the option to surrender
    bool virtual_exec_surrendered = false;
    bool within_active_analysis = false; // mark which functions do not have a closed form yet

    int callee_reg_size = 4;
    int callee_reg_count = callee_save_regs.size();

    ASTFunctionNode(std::string f_name, std::vector<ASTStatementNode*> body, size_t f_stack_size, size_t arg_stackpart_size,
                    std::vector<std::pair<std::string,std::string>> argument_list, std::string return_type, std::map<std::string, VirtualMathTerm> complexity_map);

    ASTFunctionNode()=default;

    void initialize_with_complexity_map(std::map<std::string, VirtualMathTerm> complexity_map);

    std::string compile();

    VirtualMathTerm get_complexity() override;

    std::string to_code() override;

    void set_block_level(int n);

    std::string get_class() override;

private:
    void virtual_execution();
};

#endif //H2_ASTFUNCTIONNODE_HPP
