//
// Created by ray on 17.01.23.
//


#include <utility>
#include <vector>
#include <map>
#include <cassert>

#include "virtual_math_term.hpp"
#include "LogicTerm.hpp"

static std::map<LogicTermType, LogicTermType> opposite_comparison_symbol = {{GREATER, LESS_EQUAL}, {LESS_EQUAL, GREATER},
                                                                     {LESS, GREATER_EQUAL}, {GREATER_EQUAL, LESS},
                                                                     {EQUAL, UNEQUAL}, {UNEQUAL,EQUAL}};

static std::map<LogicTermType, LogicTermType> de_morgan_opposite = {{L_AND, L_OR}, {L_OR, L_AND}};




    bool LogicTerm::is_value() const { return type == TRUE or type==false;}
    bool LogicTerm::is_combination() const { return type == L_AND or type == L_OR or type == L_NOT;}
    bool LogicTerm::is_converter() const {return type == GREATER or type ==  GREATER_EQUAL or type ==  LESS_EQUAL or type ==  LESS or type ==  EQUAL or type == UNEQUAL;}


LogicTerm::LogicTerm(LogicTermType type): type(type) {}

LogicTerm::LogicTerm(LogicTermType type, std::vector<LogicTerm> children): type(type) {
        assert(is_combination());
        logic_children = std::move(children);
    }

LogicTerm::LogicTerm(LogicTermType type, std::vector<VirtualMathTerm> children): type(type) {
        assert(is_converter());
        math_children = std::move(children);
    }


    void LogicTerm::simplify() {
        auto before = *this;
        simplify_logic();
        // keep on simplifying, since term was still changing
        if (before!=*this) simplify();
    }

    bool LogicTerm::operator==(const LogicTerm& other) const {
        if (type != other.type) return false;
        if (is_value()) return true;

        if (is_converter()) {
            return (math_children == other.math_children);
        }

        if (is_combination()) {
            if (logic_children.size() != other.logic_children.size()) return false;
            for (int i = 0; i<logic_children.size(); i++)
                if (logic_children[i] != other.logic_children[i]) return false;
            return true;
        }
        assert(false);
    }

    bool LogicTerm::operator!=(const LogicTerm& other) const {
        return !(*this == other);
    }


    void LogicTerm::copy_to_me(const LogicTerm other) {
        type = other.type;
        logic_children = other.logic_children;
        math_children = other.math_children;
    }



    void LogicTerm::simplify_logic() {
        if (is_value()) return;

        for (auto& e : logic_children) e.simplify();
        for (auto& e : math_children) e.simplify();

        if (is_combination()) {

            if (type == L_AND) {
                std::vector<LogicTerm> new_logic_children;
                for (auto& e : logic_children) {

                    // simplify true and false
                    if (e.type == FALSE) {
                        copy_to_me({FALSE});
                        return;
                    }
                    if (e.type == TRUE) continue;

                    // unify same structure into one
                    if (e.type == this->type) {
                        new_logic_children.insert(new_logic_children.end(), e.logic_children.begin(), e.logic_children.end());
                        continue;
                    }

                    new_logic_children.push_back(e);

                }
                logic_children = new_logic_children;
            }

            if (type == L_OR) {
                std::vector<LogicTerm> new_logic_children;
                for (auto& e : logic_children) {
                    if (e.type == TRUE) {
                        copy_to_me({TRUE});
                        return;
                    }
                    if (e.type == FALSE) continue;

                    // unify same structure into one
                    if (e.type == this->type) {
                        new_logic_children.insert(new_logic_children.end(), e.logic_children.begin(), e.logic_children.end());
                        continue;
                    }

                    new_logic_children.push_back(e);
                }

                logic_children = new_logic_children;
            }

            // resolve structure if it has only one child
            if (type == L_AND or type == L_OR) {
                if (logic_children.size() == 1) {
                    copy_to_me(logic_children[0]);
                }
            }

            if (type == L_NOT) {
                assert(logic_children.size() == 1);
                auto child = logic_children[0];

                // resolve double not
                if (child.type == L_NOT) {
                    copy_to_me(child.logic_children[0]);
                    return;
                }

                // reverse comparison
                if (child.is_converter()) {
                    child.type = opposite_comparison_symbol[child.type];
                    copy_to_me(child);
                    return;
                }

                // DeMorgan rule
                if (child.type == L_AND or child.type == L_OR) {
                    std::vector<LogicTerm> new_child_logic_children;
                    for (auto& e : child.logic_children) {
                        new_child_logic_children.push_back({L_NOT,{e}});
                    }
                    child.logic_children = new_child_logic_children;
                    child.type = de_morgan_opposite[child.type];
                    copy_to_me(child);
                    return;
                }
            }
        }
    }


