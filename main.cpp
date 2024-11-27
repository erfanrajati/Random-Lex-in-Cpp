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
        return {};
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

    char begin;
    file.get(begin);
    array<string, 2> result = scanner.findString(begin);

    
    // Debug
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