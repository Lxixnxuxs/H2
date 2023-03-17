//
// Created by ray on 14.03.23.
//

#ifndef H2_FILEEDITOR_HPP
#define H2_FILEEDITOR_HPP

#include <string>
#include <vector>
#include <optional>


// symbols at the front have higher priority. e.g. in " -> int" we would find a "->" and not a "-"
const std::vector<std::string> lexer_symbols = {"/*","*/","/%","%/","->","==","!=",">=","<=","=","(",")","[","]","{","}","-",
                                                "+","*","/",",",".",";","<<",">>","&","|","€","<",">","%","^","'","\""};



struct Token {
    std::string content;
    std::optional<std::pair<int, int>> text_position_boundary; // including start, excluding end

    Token(std::string content_, std::optional<std::pair<int, int>> text_position_boundary_);
};


struct Replacement {
    size_t start_index;
    size_t end_index;
    std::string replacement;

    Replacement(size_t start, size_t end, std::string replacement_);

    bool overlaps(const Replacement& other) const;

private:
    bool surrounds_start_of(Replacement const& other) const;
};

struct Tokenstream;

struct FileEditor{
    friend class Tokenstream;

    std::string file_name;
    std::string content;

    FileEditor(std::string file_name);

    FileEditor();

    void enqueue_change(Replacement r);
    /* inclusive [start_index, end_index) exclusive*/

    void apply_changes();

    std::pair<int,int> line_and_position_of_index(int index);

    Tokenstream get_line(int nr);

    std::string get_line_as_string(int nr);

    std::string get_error_position_information(Tokenstream t);

private:
    std::vector<Replacement> changes;

    std::vector<Token> tokens;

    std::vector<std::pair<int,int>> line_break_positions;      // index, token


    void lexing();
};

#endif //H2_FILEEDITOR_HPP
