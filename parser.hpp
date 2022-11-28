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
#include "ParsedFunction.hpp"

struct Parser {
    std::string file_content;

    // trim from start
    static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    // trim from end
    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    // trim from both ends
    static inline std::string &trim(std::string s) {
        return ltrim(rtrim(s));
    }

    /**
     * Computes the position of the first character present in the next occurrence of @param s
     * starting from position @param i
     */
    size_t find_next_string(size_t i, std::string s) {
        return file_content.find(s, i);
    }

    /**
     * Computes the number of characters between position @param i and the next occurrence of the character @param c
     * starting from i, excluding position i
     */
    size_t num_chars_until(size_t i, char c) {
        return file_content.find(c, i) - i;
    }

    /**
     * Computes the position of the next non-whitespace (newline, whitespace, tab) character starting from @param i
     */
    size_t next_non_whitespace_character(size_t i) {
        size_t j = i;
        while (j < file_content.size() &&
              (file_content.at(j) == '\n' || file_content.at(j) == ' ' || file_content.at(j) == '\r')) {
            j++;
        }
        return j;
    }

    /**
     * Computes the position of the corresponding closing bracket starting from @param i = (position+1) of its opening
     * bracket. If @param isCurly true, matches for curly brackets. If false matches for normal brackets
     */
    size_t pos_matching_bracket(size_t i, bool isCurly) {
        char b = isCurly ? '{' : '(';
        char c = isCurly ? '}' : ')';

        size_t counter = 0, j = i;

        while (j < file_content.size()) {
            if (file_content.at(j) == c && counter == 0) {
                break;
            } else if (file_content.at(j) == c && counter > 0) {
                counter--;
            } else if (file_content.at(j) == b) {
                counter++;
            }

            j++;
        }

        return j < file_content.size() ? j : -1;
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

public:
    Parser() {};

    Parser(std::string& filename) {
        std::ifstream f(filename);

        read(f);
    };

    void read(std::ifstream& filestream) {
        std::stringstream buffer;

        buffer << filestream.rdbuf();

        file_content =  buffer.str();
    }

    std::string& get_file_content() {
        return file_content;
    }

    std::vector<ParsedFunction> split_by_functions() {
        std::vector<ParsedFunction> functions;

        size_t function_pos = find_next_string(0, "def");

        while (function_pos != std::string::npos) {
            size_t f_name_pos = next_non_whitespace_character(function_pos + 3);
            size_t f_name_end_pos = num_chars_until(f_name_pos, '(');

            size_t f_args_pos = find_next_string(f_name_pos, "(") + 1;
            size_t f_args_end_pos = pos_matching_bracket(f_args_pos, false);

            size_t f_type_pos =
                    next_non_whitespace_character(next_non_whitespace_character(f_args_end_pos + 1) + 2);
            size_t f_type_end_pos = find_next_string(f_type_pos, " ");

            size_t f_block_pos = next_non_whitespace_character(f_type_end_pos) + 1;
            size_t f_block_end_pos = pos_matching_bracket(f_block_pos + 1, true) - 1;

            ParsedFunction f(file_content.substr(f_name_pos, f_name_end_pos),
                             file_content.substr(f_args_pos, f_args_end_pos - f_args_pos),
                             file_content.substr(f_type_pos, f_type_end_pos - f_type_pos),
                             extract_statements(
                             file_content.substr(f_block_pos, f_block_end_pos - f_block_pos + 1)));

            functions.push_back(f);

            function_pos = find_next_string(function_pos + 1, "def");
        }

        return functions;
    }
};

#endif //H2_PARSER_HPP