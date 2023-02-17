//
// Created by ray on 16.02.23.
//

#include "ASTCommentNode.hpp"
#include "../global_information.hpp"

ASTCommentNode::ASTCommentNode(std::string text): text(text) {}

std::string ASTCommentNode::get_class() {return "Comment";}

std::string ASTCommentNode::to_code() {return get_indentation(block_level)+ "/*"+text+" */\n";}

std::string ASTCommentNode::compile() {return "";}

VirtualMathTerm ASTCommentNode::get_complexity() {return VirtualMathTerm(0);}
