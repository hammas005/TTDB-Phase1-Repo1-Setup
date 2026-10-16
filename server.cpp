#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include <map>
#include <sstream>
#include <stack>
#include <vector>

using namespace std;

// Global Call Table to store function names and their byte offsets
// Maps function names to their absolute byte offsets in the binary payload
map<string, uint32_t> callTable;

// Global Memory Map to track variable states
// Tracks the current integer value of active variables
map<string, int32_t> memory;

// NEW: Time-Travel History to store memory snapshots after every command
vector<map<string, int32_t>> memoryHistory;

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
    // Validates source code for illegal function nesting and missing ends
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
bool resolve() {
    // Serializes text commands into binary format and builds Call Table
    ifstream inFile("source.bin");
    ofstream outFile("resolve.bin", ios::binary);
    
    if (!inFile.is_open() || !outFile.is_open()) {
        cout << "Error: Could not open files for Pass 0x1." << endl;
        return false;
    }

    string line;
    uint32_t currentOffset = 0;

    // Read line by line to calculate offsets and write binary data
    while (readSourceLine(inFile, line)) {
        string cmd = firstWord(line);
        uint32_t size = line.length();
        
        // If it's a function, extract the name and save its memory location
        if (cmd == "func") {
            size_t firstSpace = line.find(' ');
            size_t secondSpace = line.find(' ', firstSpace + 1);
            if (firstSpace != string::npos && secondSpace != string::npos) {
                string funcName = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
                callTable[funcName] = currentOffset;
            }
        }

        // Write [offset][size][string] sequentially to the binary file
        outFile.write(reinterpret_cast<char*>(&currentOffset), sizeof(currentOffset));
        outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
        outFile.write(line.c_str(), size);
        
        // Move the offset forward by the exact size of the data we just wrote
        currentOffset += sizeof(currentOffset) + sizeof(size) + size;
    }
    
    inFile.close();
    outFile.close();
    
    cout << "Pass 0x1: Resolve Passed Successfully (resolve.bin created)!" << endl;
    return true;
}
// PASS 0x2: EXECUTION SIMULATION
bool execute() {
    // Reads binary payload, executes instructions, and tracks memory state
    ifstream binFile("resolve.bin", ios::binary);
    
    if (!binFile.is_open()) {
        cout << "Error: Could not open resolve.bin for execution." << endl;
        return false;
    }

    uint32_t offset, size;
    // Maintains return addresses for nested function execution
    stack<uint32_t> callStack; 
    
    while (binFile.read(reinterpret_cast<char*>(&offset), sizeof(offset))) {
        binFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        
        string instruction;
        instruction.resize(size);
        binFile.read(&instruction[0], size);
        
        stringstream ss(instruction);
        string cmd;
        ss >> cmd;

        if (cmd == "set") {
            string varName;
            int32_t value;
            ss >> varName >> value;
            memory[varName] = value; 
        }
        else if (cmd == "add") {
            string varName;
            int32_t value;
            ss >> varName >> value;
            memory[varName] += value;
        }
        else if (cmd == "print") {
            string varName;
            ss >> varName;
            cout << varName << " = " << memory[varName] << endl; 
        }
        else if (cmd == "call") {
            string funcName;
            ss >> funcName;
            if (callTable.find(funcName) != callTable.end()) {
                callStack.push(binFile.tellg());      
                binFile.seekg(callTable[funcName]);   
            } else {
                cout << "Execution Error: Function '" << funcName << "' not found." << endl;
            }
        }
        else if (cmd == "func_end") {
            if (!callStack.empty()) {
                binFile.seekg(callStack.top()); 
                callStack.pop();                
            }
        }

        // NEW: Take a snapshot of memory after executing this instruction
        memoryHistory.push_back(memory);
    }
    
    binFile.close();
    cout << "Pass 0x2: Execution engine ran successfully!" << endl;
    return true;
}


// PASS 0x3: TIME-TRAVEL INTERACTIVE PROMPT
void interactiveTimeTravel() {
    // Provides CLI interface for navigating historical memory states
    cout << "\n=== Time-Travel Debugger Active ===" << endl;
    cout << "Total execution steps recorded: " << memoryHistory.size() << endl;
    
    string input;
    while (true) {
        cout << "\nEnter a step number to time-travel (or type 'exit' to quit): ";
        cin >> input;
        
        if (input == "exit") break;
        
        int step;
        try {
            step = stoi(input); 
        } catch (...) {
            cout << "Invalid input. Please enter a valid number." << endl;
            continue;
        }
        
        if (step < 1 || step > (int)memoryHistory.size()) {
            cout << "Error: Step out of bounds." << endl;
            continue;
        }
        
        // NEW: Retrieve and print the memory state at the requested step!
        cout << "--- Memory State at Step " << step << " ---" << endl;
        map<string, int32_t> snapshot = memoryHistory[step - 1]; // -1 because vectors are 0-indexed
        
        if (snapshot.empty()) {
            cout << "(Memory is empty)" << endl;
        } else {
            // Loop through all variables in the snapshot and print them
            for (auto const& [varName, value] : snapshot) {
                cout << varName << " = " << value << endl;
            }
        }
        cout << "-------------------------------" << endl;
    }
    
    cout << "Exiting Time-Travel Debugger. Goodbye!" << endl;
}

int32_t main() {
    // --- MAIN ENTRY POINT ---
    cout << "--- Starting Time-Travel Debugger Server ---" << endl;
    
    if (!validityCheck()) {
        cout << "Server shutting down due to validation errors." << endl;
        return 1;
    }

    if (!resolve()) {
        cout << "Server shutting down due to resolve errors." << endl;
        return 1;
    }
    
    if (!execute()) {
        cout << "Server shutting down due to execution errors." << endl;
        return 1;
    }
    
    // Run Pass 0x3
    interactiveTimeTravel();
    
    return 0;
}