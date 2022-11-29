//
// Created by ray on 28.11.22.
//
#include <iostream>
#include <string>
#include <map>

typedef std::string string;


struct Accumulator {
    string code;
    std::map<string,int> bookmarks; // pointing to a position in the string

    Accumulator(){
        code =""; //"insert initial code\n"; // insert initial code
    }

    void push(const string& s, bool add_nextline = true){
        code+=s;
        if (add_nextline) code+='\n';
    }

    void set_bookmark(const string& name){
        bookmarks[name] = code.size();
    }

    void insert_at(const string& bookmark, const string& text, bool add_nextline = true){
        int pos = bookmarks[bookmark];
        string nextl = add_nextline ? "\n" : "";
        code = code.substr(0, pos) + text + nextl +
               code.substr(pos, code.size() - pos);

        //if (add_nextline) code+='\n';
    }

};

/*
int main(){
    //string s = "uselesstext";
    //std::cout << s.substr(3,3) << std::endl;
    Accumulator a;
    a.push("addi $34 #rax");
    a.set_bookmark("p1");
    a.push("muli $34 #rax");
    a.insert_at("p1","in between!");
    std::cout << a.code;
}
 */