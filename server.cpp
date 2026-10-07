#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include <map>

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

// PASS 0x1: RESOLVE
map<string, uint32_t> callTable; // Stores function names and their byte offsets

bool resolve() {
    ifstream inFile("source.bin");
    ofstream outFile("resolve.bin", ios::binary);
    
    if (!inFile.is_open() || !outFile.is_open()) {
        cout << "Error opening files for Pass 0x1." << endl;
        return false;
    }

    string line;
    uint32_t currentOffset = 0;

    while (readSourceLine(inFile, line)) {
        string cmd = firstWord(line);
        uint32_t size = line.length();
        
        // If it's a function declaration, save its memory offset in the Call Table
        if (cmd == "func") {
            size_t firstSpace = line.find(' ');
            size_t secondSpace = line.find(' ', firstSpace + 1);
            string funcName = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
            callTable[funcName] = currentOffset;
        }

        // Write [offset][size][string] to resolve.bin securely
        outFile.write(reinterpret_cast<char*>(&currentOffset), sizeof(currentOffset));
        outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
        outFile.write(line.c_str(), size);
        
        // Calculate the starting position for the next line
        currentOffset += sizeof(currentOffset) + sizeof(size) + size;
    }
    
    inFile.close();
    outFile.close();
    
    cout << "Pass 0x1: Resolve Passed Successfully (resolve.bin created)!" << endl;
    return true;
}

iint32_t main() {
    cout << "--- Starting Time-Travel Debugger Server ---" << endl;
    
    // Execute Pass 0x0
    if (!validityCheck()) {
        cout << "Server shutting down due to validation errors." << endl;
        return 1;
    }

    // Execute Pass 0x1
    if (!resolve()) {
        cout << "Server shutting down due to resolve errors." << endl;
        return 1;
    }

    return 0;
}