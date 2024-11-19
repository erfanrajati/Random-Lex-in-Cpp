#include <iostream>
#include <string>
#include <map>

using namespace std;

class Scanner {
public:
    Scanner() {
        cout << "Scanner object initialized" << endl;
    }

    map<char, char> Tokenize(string s) {
        map<char, char> m;
        m['a'] = 'b';

        return m;
    }
};

int main() {
    cout << "Hello, World!" << endl;;

    Scanner scn;
    return 0;
}