#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include <map>

using namespace std;

// Global Call Table to store function names and their byte offsets
map<string, uint32_t> callTable;

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

// PASS 0x0: VALIDITY CHECK
bool validityCheck() {
    ifstream file("source.bin");
    
    // Check if the file exists and can be opened
    if (!file.is_open()) {
        cout << "Error: Could not open source.bin" << endl;
        return false;
    }

    // We will add the reading loop here in the next commit
    
    file.close();
    return true;
}

int32_t main() {
    cout << "Server starting..." << endl;
    
    // Run Pass 0x0
    if (!validityCheck()) {
        cout << "Server shutting down due to validation errors." << endl;
        return 1;
    }
    
    return 0;
}