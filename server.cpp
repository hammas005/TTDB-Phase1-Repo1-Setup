#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>

using namespace std;

// Helper 1: Reads the file line by line, skipping blank lines
bool readSourceLine(ifstream &in, string &out) {
    while (getline(in, out)) {
        if (out != "") return true;
    }
    return false;
}

// Helper 2: Extracts the very first word from a string
string firstWord(const string &line) {
    size_t spaceIndex = line.find(' ');
    if (spaceIndex == string::npos) return line;
    return line.substr(0, spaceIndex);
}

int32_t main() {
    cout << "Server starting..." << endl;
    return 0;
}