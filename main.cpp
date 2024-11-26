#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <array>

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
private:
    ifstream& fileIN;

    map<char, char> Tokenize(string s) {
        map<char, char> m;
        m['a'] = 'b';

        return m;
    }

    array<string, 2> findVar() { // returns the token, first str is the token and second is the token type
        // the cursor in the fileIN must be on a $ sign at this point.
        
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
        string token = "";
        while (fileIN.get(ch) && state != '3') {
            int ascii = static_cast<int>(ch);
            if (ascii >= 66 && ascii <= 90)
                edge = 'U';
            else if (ascii >= 96 && ascii <= 122)
                edge = 'L';
            else if (ascii >= 48 && ascii <= 57)
                edge = 'D';
            else 
                edge = '#';

            // cout << state << edge << endl; // Debug
            state = products.at({state, edge}); // throws error if key does not exist.
            
            token += ch;
        }

        // Backtrack
        fileIN.unget();
        token.pop_back();

        return {token, "id"};
    }
    
    array<int, 2> findOpr(int first, int forward) {
        return {};
    }
    array<int, 2> findDT(int first, int forward) { // DataType
        return {};
    }
    array<int, 2> findKW(int first, int forward) { // Key Word
        return {};
    }
    array<int, 2> findDel(int first, int forward) { // delimiters
        return {};
    }

public:

    Scanner(ifstream& fileIN) : fileIN(fileIN) { // pass by reference
        // char ch;
        // while (this->fileIN.get(ch))
        //     cout << ch;
    }
};

int main() {
    string fileName = "test.txt";
    ifstream file(fileName);
    if (!file.is_open())
        cout << "Couldn't find file: " + fileName << endl;
        return 0; 

    Scanner scn(file);
    file.close();
    return 0;
}