//
// Created by ray on 14.03.23.
//

#include <fstream>
#include "FileEditor.hpp"
#include <algorithm>
#include <utility>
#include "Tokenstream.hpp"

Token::Token(std::string content_, std::optional<std::pair<int, int>> text_position_boundary_):
        content(std::move(content_)), text_position_boundary(std::move(text_position_boundary_)) {}

bool Replacement::overlaps(const Replacement &other) const{
    return surrounds_start_of(other) || other.surrounds_start_of(*this);
}

bool Replacement::surrounds_start_of(const Replacement &other) const {
    return other.start_index>=start_index && other.start_index < end_index;
}

Replacement::Replacement(size_t start, size_t end, std::string replacement_): start_index(start), end_index(end),replacement(std::move(replacement_)) {}


FileEditor::FileEditor(std::string file_name): file_name(file_name) {
    std::ifstream file(file_name);
    content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    lexing();
}

FileEditor::FileEditor() = default;

void FileEditor::enqueue_change(Replacement r) {
    if (r.end_index < r.start_index) throw std::invalid_argument("ERROR: FileEditor inserting change of negative length"); // empty change

    // ensure this change does not collide with previous ones
    for (const auto& e : changes) {
        if (r.overlaps(e)) throw std::invalid_argument("ERROR: Colliding changes at FileEditor");
    }

    changes.push_back(r);
}

void FileEditor::apply_changes() {

    // sort changes by start index
    std::sort(changes.begin(), changes.end(),[](Replacement const& a, Replacement const& b){
        return a.start_index > b.start_index;
    });

    // apply later changes first to avoid invalidating later changes
    for (const auto& e : changes) {
        auto replacement_start = content.begin();
        std::advance(replacement_start, e.start_index);

        auto replacement_end = content.begin();
        std::advance(replacement_end, e.end_index);

        content.erase(replacement_start, replacement_end);
        content.insert(e.start_index, e.replacement);
    }

    // writing back to file
    if (file_name != ""){
        std::ofstream file(file_name);
        file << content;
        file.close();
    }

}

void FileEditor::lexing() {

    int token_count = 0;

    auto create_token_from_boundaries = [&](size_t start, size_t end) {
        tokens.push_back({content.substr(start,end-start), {{start, end}}});
        token_count++;
    };

    int last_pos = 0;
    int current_pos = 0;

    while (current_pos < content.size()){
        if (content.at(current_pos) == '\n') line_break_positions.emplace_back(current_pos,token_count);

        if (std::isspace(content.at(current_pos))){ //split at whitespace
            if (last_pos < current_pos){ // take if there was something before
                create_token_from_boundaries(last_pos, current_pos);//tokens.push_back({content.substr(last_pos,current_pos-last_pos), {{last_pos, current_pos}}});
            }
            last_pos = current_pos + 1; // throw away the whitespace
        } else {
            for (const std::string& symbol : lexer_symbols){
                size_t pos = content.find(symbol, current_pos);
                if (pos == current_pos){
                    if (last_pos < current_pos){ // take only if not empty
                        create_token_from_boundaries(last_pos, current_pos);//tokens.push_back({content.substr(last_pos,current_pos-last_pos), {{last_pos, current_pos}}});
                    }
                    create_token_from_boundaries(current_pos, current_pos + symbol.size());//tokens.push_back({content.substr(current_pos,symbol.size()),{{current_pos, current_pos+symbol.size()}}});
                    current_pos+=symbol.size();
                    last_pos = current_pos;
                    current_pos--; //neutralize current_pos++
                    break;
                }
            }
        }
        current_pos++;
    }

    // include last token which would be forgotten otherwise
    if (last_pos < current_pos) create_token_from_boundaries(last_pos, current_pos); //{tokens->push_back(content.substr(last_pos,current_pos-last_pos));}

    //remove_comments(res); // comments are removed by default

}

std::pair<int, int> FileEditor::line_and_position_of_index(int index) {
    int i = 0;
    while (i<line_break_positions.size() && line_break_positions[i].first < index) i++;
    return {i+1, index - line_break_positions[i-1].first}; // 1-indexing for lines
}

Tokenstream FileEditor::get_line(int nr) {
    Tokenstream res = Tokenstream(*this);
    res.end_ = res.begin_;
    std::advance(res.end_, line_break_positions[nr-1].second);
    std::advance(res.begin_,line_break_positions[nr-2].second);

    return res;
}

std::string FileEditor::get_line_as_string(int nr) {
    int begin = line_break_positions[nr-2].first;
    int end = line_break_positions[nr-1].first;

    return content.substr(begin,end-begin);
}


std::string FileEditor::get_error_position_information(Tokenstream t) {
    //try {t.file_editor->get_line(1);}
    //catch (...) { throw std::invalid_argument("this Tokenstream has no file editor");}

    auto token = t.get_token();
    int absolute_position = token.text_position_boundary.value().first;
    auto [line, position] = t.file_editor->line_and_position_of_index(absolute_position);
    return "\nline " + std::to_string(line)+ " at position "+std::to_string(position)+":\n"+
           t.file_editor->get_line_as_string(line)+"\n"+std::string(position,' ')+"^\n";
}

