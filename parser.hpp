#ifndef H2_PARSER_HPP
#define H2_PARSER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <map>

namespace Parser {
    static char operators[] = {'+', '-', '*', '/'};
    static std::string regs[] = {"%rcx", "%r8", "%r9", "%r10", "%r11"};
    static std::map<std::string, size_t> type_to_size = { {"int", 8} };

    // trim from start
    inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    // trim from end
    inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    // trim from both ends
    inline std::string &trim(std::string s) {
        return ltrim(rtrim(s));
    }

    /**
     * Computes the position of the first character present in the next occurrence of @param s
     * starting from position @param i
     */
    size_t find_next_string(std::string f, size_t i, std::string s) {
        return f.find(s, i);
    }

    /**
     * Computes the number of characters between position @param i and the next occurrence of the character @param c
     * starting from i, excluding position i
     */
    size_t num_chars_until(std::string f, size_t i, char c) {
        return f.find(c, i) - i;
    }

    /**
     * Computes the position of the next non-whitespace (newline, whitespace, tab) character starting from @param i
     */
    size_t next_non_whitespace_character(std::string f, size_t i) {
        size_t j = i;
        while (j < f.size() &&
              (f.at(j) == '\n' || f.at(j) == ' ' || f.at(j) == '\r')) {
            j++;
        }
        return j;
    }

    /**
     * Computes the position of the corresponding closing bracket starting from @param i = (position+1) of its opening
     * bracket. If @param isCurly true, matches for curly brackets. If false matches for normal brackets
     */
    size_t pos_matching_bracket(std::string f, size_t i, bool isCurly=false) {
        char b = isCurly ? '{' : '(';
        char c = isCurly ? '}' : ')';

        size_t counter = 0, j = i;

        while (j < f.size()) {
            if (f.at(j) == c && counter == 0) {
                break;
            } else if (f.at(j) == c && counter > 0) {
                counter--;
            } else if (f.at(j) == b) {
                counter++;
            }

            j++;
        }

        return j < f.size() ? j : -1;
    }

    std::vector<std::string> extract_statements(std::string s) {
        size_t pos = 0, pos_end;
        std::vector<std::string> statements;

        pos_end = s.find(';', pos);

        while (pos_end != std::string::npos) {
            statements.push_back(trim(s.substr(pos, pos_end - pos)));

            pos = pos_end + 1;
            pos_end = s.find(';', pos);
        }

        return statements;
    }

    std::string s_substring(std::string& s, size_t from, size_t to) {
        return s.substr(from, to - from);
    }

    std::string substring_between_brackets(std::string s, size_t i, bool isCurly = false) {
        return s_substring(s, i + 1, pos_matching_bracket(s, i + 1, isCurly));
    }

    std::string read_corresponding_bracket(std::string& s, size_t i, bool isCurly = false, bool stripped = true) {
        /* mutating the string s and extract all the content within the brackets and cutting it out of s*/

        size_t pos = pos_matching_bracket(s, i + 1,isCurly);
        auto res = s_substring(s, i + 1 , pos);
        s = s_substring(s,pos+1,s.size());

        if (stripped) {
            Parser::ltrim(s);
            res = Parser::trim(res);
        }

        return res;
    }


    size_t find_operator(std::string s, size_t i = 0) {
        for (char op : operators) {
            size_t pos = s.find(op, i);
            if (pos != std::string::npos) {
                return pos;
            }
        }

        return std::string::npos;
    }

    std::string read_until(std::string& s, char c = ' ', bool stripped = true) {
        size_t pos = s.find(c);
        std::string res = s.substr(0, pos);
        s = s_substring(s, pos + 1, s.size());

        if (stripped) {
            Parser::ltrim(s);
            res = Parser::trim(res);
        }

        return res;
    }
};

#endif //H2_PARSER_HPP