//
// Created by ray on 20.01.23.
//
#include "ASTNode.hpp"

#include "../global_information.hpp"

std::string get_indentation(int level) {
    return std::string(level * block_indentation,' ');
}



void ASTNode::set_block_level(int n = 0){
    block_level = n;
}

