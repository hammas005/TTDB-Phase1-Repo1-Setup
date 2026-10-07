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

// PASS 0x0: VALIDITY CHECK
bool validityCheck() {
    ifstream file("source.bin");
    if (!file.is_open()) {
        cout << "Error: Could not open source.bin" << endl;
        return false;
    }

    string line;
    int funcCount = 0;
    bool insideFunc = false;

    // Read the file line by line
    while (readSourceLine(file, line)) {
        string cmd = firstWord(line);
        
        if (cmd == "func") {
            if (insideFunc) {
                cout << "Validation Error: Nested functions are not allowed." << endl;
                return false;
            }
            insideFunc = true;
            funcCount++;
        } else if (cmd == "func_end") {
            if (!insideFunc) {
                cout << "Validation Error: 'func_end' found without a matching 'func'." << endl;
                return false;
            }
            insideFunc = false;
        }
    }
    file.close();

    // Final checks
    if (insideFunc) {
        cout << "Validation Error: Missing 'func_end' at the end of the file." << endl;
        return false;
    }
    if (funcCount == 0) {
        cout << "Validation Error: No functions found in the file." << endl;
        return false;
    }

    cout << "Pass 0x0: Validity Check Passed Successfully!" << endl;
    return true;
}

int32_t main() {
    cout << "--- Starting Time-Travel Debugger Server ---" << endl;
    
    // Execute Pass 0x0
    if (!validityCheck()) {
        cout << "Server shutting down due to validation errors." << endl;
        return 1;
    }

    return 0;
}