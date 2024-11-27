#include <iostream>
#include <stdexcept>
#include <functional>
#include <string>
#include <fstream>
#include <map>
#include <array>
#include <vector>

using namespace std;


class Scanner {
// private:
public:
    ifstream& fileIN;
    vector<array<string, 2>> tokens;
    // array<function<array<string, 2>>(), 7> methods = {
    //     findVar, findOpr, findDT, 
    //     findKW, findDel, findString,
    //     findNumber
    // }; 
    // to call them iteratively if any one of them failed.
    // code can be ooptimized by changing the order of these functions.


    array<string, 2> findID(char begin) { 
        // tokenized variables (begin = $), functions(begin = &), classes(begin = @)

        // grammar products map for finding variables
        map<
            array<char, 2>,
            char // next state
        > products;

        products[{'1', 'U'}] = '1'; // U stands for upper case
        products[{'1', 'L'}] = '1'; // L stands for lower case
        products[{'1', 'D'}] = '1'; // D stands for digit 
        products[{'1', '"'}] = '2'; // # stands for other, 3 is accepted state, needs backtrack
        
        char state, edge, ch;
        state = '1';
        string token(1, begin);
        while (fileIN.get(ch) && state != '3') {
            cout << "Lex Log: Currently checking " << ch << endl; // Debug
            int ascii = static_cast<int>(ch);
            if (ascii >= 65 && ascii <= 90)
                edge = 'U';
            else if (ascii >= 97 && ascii <= 122)
                edge = 'L';
            else if (ascii >= 48 && ascii <= 57)
                edge = 'D';
            else 
                edge = '#';

            cout << "State Diagram Log: " << state << edge << endl; // Debug
            state = products.at({state, edge}); // throws error if token must be DECLINED.
            
            token += ch;
        }

        // Backtrack
        fileIN.unget();
        token.pop_back();
        
        string type;
        switch (begin) {
            case '$': type = "var"; break;
            case '&': type = "func"; break;
            case '@': type = "class"; break;
        }
        return {token, type};
    }

    array<string, 2> findOpr() {
        // the cursor must be on the first character of the operator.

        // grammar products map for finding variables
        map<
            array<string, 2>,
            string // next state
        > products;

        products[{"0", "i"}] = "1"; 
        products[{"1", "n"}] = "2"; 
        products[{"2", "c"}] = "ACCEPTED"; // inc accepted
        products[{"0", "d"}] = "4"; 
        products[{"4", "i"}] = "5"; 
        products[{"5", "v"}] = "ACCEPTED"; // div accepted
        products[{"4", "e"}] = "7"; 
        products[{"7", "c"}] = "ACCEPTED"; // dec accepted
        products[{"0", "+"}] = "ACCEPTED";
        products[{"0", "-"}] = "ACCEPTED";
        products[{"0", "*"}] = "ACCEPTED";
        products[{"0", "/"}] = "ACCEPTED";
        products[{"0", "^"}] = "ACCEPTED";
        products[{"0", "="}] = "14";
        products[{"14", "#"}] = "BACKTRACK"; // = accepted, backtrack
        products[{"14", "="}] = "ACCEPTED"; // == accepted
        products[{"0", ">"}] = "17";
        products[{"17", "#"}] = "BACKTRACK"; // > accepted, backtrack
        products[{"17", "="}] = "ACCEPTED"; // >= accepted
        products[{"0", "<"}] = "20";
        products[{"20", "#"}] = "BACKTRACK"; // = accepted, backtrack
        products[{"20", "="}] = "ACCEPTED"; // <= accepted
        products[{"20", ">"}] = "ACCEPTED"; // <> accepted

        // for a real compiler, it is needed to have separated acceptance states for each token type.
        // but for code simplicity here, I only consider tokens in one of three ways: 
        // ACCEPTED, BACKTRACK, DECLINED.

        char ch;
        string state = "0";
        string token;
        while (fileIN.get(ch) && (state != "ACCEPTED" && state != "BACKTRACK")) {
            // cout << "Lex Log: Currently checking " << ch << endl; // Debug
            string edge(1, ch);
            try {
                state = products.at({state, edge});
            } catch (const out_of_range& e) {
                edge = "#"; // if it got any other character
                state = products.at({state, edge}); // throws error if token must be DECLINED.
            }
            token += edge;
            cout << "Token: " << token << endl;
        }

        if (state == "BACKTRACK") {
            fileIN.unget();
            token.pop_back();
        }
    
        return {token, "opr"};
    }

    array<string, 2> findDT() { // DataType
        return {};
    }
    array<string, 2> findKW() { // Key Word
        // Lex has gotten any char in {f, c, r, u, w, i, e} (first letter of all keywords)
        // grammar products map for finding variables
        map<
            array<string, 2>,
            string // next state
        > products;

        products[{"0", "f"}] = "1"; // checks for func / for
        products[{"1", "o"}] = "8";        
        products[{"8", "r"}] = "9"; // for accepted
        products[{"1", "u"}] = "10";       
        products[{"10", "n"}] = "11";       
        products[{"11", "c"}] = "ACCEPTED"; // func accepted  

        products[{"0", "c"}] = "2"; // checks for class
        products[{"2", "l"}] = "13";
        products[{"13", "a"}] = "14";
        products[{"14", "s"}] = "15";
        products[{"15", "s"}] = "ACCEPTED"; // class accepted

        products[{"0", "r"}] = "3"; // checks for repeat
        products[{"3", "e"}] = "17";
        products[{"17", "p"}] = "18";
        products[{"18", "e"}] = "19";
        products[{"19", "a"}] = "20";
        products[{"20", "t"}] = "ACCEPTED"; // repeat accepted

        products[{"0", "u"}] = "4"; // checks for until
        products[{"4", "n"}] = "22";  
        products[{"22", "t"}] = "23";  
        products[{"23", "i"}] = "24";  
        products[{"24", "l"}] = "ACCEPTED"; // until accepted

        products[{"0", "w"}] = "5"; // checks for while
        products[{"5", "h"}] = "26"; 
        products[{"26", "i"}] = "27"; 
        products[{"27", "l"}] = "28"; 
        products[{"28", "e"}] = "ACCEPTED"; // while accepted

        products[{"0", "i"}] = "6"; // checks for if
        products[{"6", "f"}] = "ACCEPTED"; // if accepted

        products[{"0", "e"}] = "7"; // checks for else / elif
        products[{"7", "l"}] = "31"; 
        products[{"31", "s"}] = "32"; 
        products[{"32", "e"}] = "ACCEPTED"; // else accepted

        products[{"7", "l"}] = "34"; 
        products[{"34", "i"}] = "35"; 
        products[{"35", "f"}] = "ACCEPTED"; // elif accepted

        char ch;
        string state = "0";
        string token = "";
        while (fileIN.get(ch) && state != "ACCEPTED") {
            string edge(1, ch);
            state = products.at({state, edge});
            token += ch;
        }

        return {token, "KeyWord"};
    }

    array<string, 2> findDel() { // delimiters
        array<char, 7> delimiters = {'[', ']', '{', '}', ';', '(', ')'};

        char ch;
        while (fileIN.get(ch)) {
            for (char c : delimiters) {
                if (ch == c) {
                    string s(1, ch);
                    return {s, "del"};
                }
            }
            throw out_of_range("Not a Delimiter");
        }
        return {};
    }

    array<string, 2> findString(char begin) { // string literals
        cout << "Lex Log: Running findString" << endl;
        // Lex must've gotten a double quotation (") at this point

        // grammar products map for finding variables
        map<
            array<char, 2>,
            char // next state
        > products;

        products[{'1', 'A'}] = '1'; // A stands for Ascii allowed characters
        products[{'1', '"'}] = '2'; // " is the end of string

        
        char state, edge, ch;
        state = '1';
        string token(1, begin);
        while (fileIN.get(ch) && state != '2') {
            cout << "Lex Log: Currently checking " << ch << endl; // Debug
            edge = (ch == '"') ? ch : 'A';

            cout << "State Diagram Log: " << state << edge << endl; // Debug

            state = products.at({state, edge}); // throws error if token must be DECLINED.
            token += ch;
        }

        return {token, "str"};
    }
    
    array<string, 2> findNumber() { // numberic literals
        return {};
    }

// public:

    Scanner(ifstream& fileIN) : fileIN(fileIN) { // pass by reference
        // char ch;
        // while (this->fileIN.get(ch))
        //     cout << ch;
    }

    vector<array<string, 2>> scan() { // where everything gets connected
        char ch;
        while (fileIN.get(ch)) {
            if (ch == '$' || ch == '&' || ch == '@') 
                try {
                    tokens.push_back(this -> findID(ch));
                } catch (const out_of_range& e) {
                    cout << e.what();
                }
        }

        return tokens;
    }
};


int main() {
    string fileName = "test.txt";
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Couldn't find file: " + fileName << endl;
        return 0; 
    }

    Scanner scanner(file);
    // vector<array<string, 2>> result = scanner.scan();


    
    // Debug
    
    char begin;
    // file.get(begin);
    array<string, 2> result = scanner.findKW();

    // for (array token:result) {
    //     for (string s:token) {
    //         cout << s + " ";
    //     }
    //     cout << endl;
    // }

    for (string s : result) 
        cout << s << ' ';

    file.close();
    return 0;
}