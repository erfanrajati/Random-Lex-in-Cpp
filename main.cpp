#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <array>
#include <vector>

using namespace std;

class StateDiagram {
private:
    map<
        array<char, 2>,
        char // next state
    > products;

public:
    StateDiagram(map<array<char, 2>, char> products) : products(products) {} ;
};


class Scanner {
// private:
public:
    ifstream& fileIN;
    vector<array<string, 2>> tokens;

    map<char, char> Tokenize(string s) {
        map<char, char> m;
        m['a'] = 'b';

        return m;
    }

    array<string, 2> findVar() { // returns the token, first str is the token and second is the token type
        // the cursor in the fileIN must be right after a $ sign at this point.

        // grammar products map for finding variables
        map<
            array<char, 2>,
            char // next state
        > products;

        products[{'1', 'U'}] = '2'; // U stands for upper case
        products[{'2', 'L'}] = '2'; // L stands for lower case
        products[{'2', 'D'}] = '2'; // D stands for digit 
        products[{'2', '#'}] = '3'; // # stands for other, 3 is accepted state, needs backtrack
        
        char state, edge, ch;
        state = '1';
        string token = "";
        while (fileIN.get(ch) && state != '3') {
            cout << "Currently checking: " << ch << endl; // Debug
            int ascii = static_cast<int>(ch);
            if (ascii >= 65 && ascii <= 90)
                edge = 'U';
            else if (ascii >= 97 && ascii <= 122)
                edge = 'L';
            else if (ascii >= 48 && ascii <= 57)
                edge = 'D';
            else 
                edge = '#';

            cout << state << edge << endl; // Debug
            state = products.at({state, edge}); // throws error if token must be DECLINED.
            
            token += ch;
        }

        // Backtrack
        fileIN.unget();
        token.pop_back();
    
        return {token, "id"};
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
            // cout << "Currently checking: " << ch << endl; // Debug
            string edge(1, ch);
            try {
                state = products.at({state, edge});
            } catch (const std::out_of_range& e) {
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
        return {};
    }
    array<string, 2> findString() { // string literals
        return {};
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

    void scan() { // where everything gets connected
        // a list of all the methods, iterate and call, catch the exception.
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
    array<string, 2> result = scanner.findOpr();

    for (string s:result)
        cout << s << endl;

    file.close();
    return 0;
}