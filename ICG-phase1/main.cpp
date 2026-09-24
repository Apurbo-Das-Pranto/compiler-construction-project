#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "antlr4-runtime.h"
#include "CSubset_2205091Lexer.h"
#include "CSubset_2205091Parser.h"
#include "2205091_visitor.h"

using namespace antlr4;
using namespace std;

ofstream lexLogFile; 

// Helper function to trim whitespaces
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Helper to check if a line is a label (e.g. L1:)
bool isLabel(const string& line) {
    string t = trim(line);
    return !t.empty() && t.back() == ':';
}

// Peephole Optimization Function (Erasing Unoptimized Lines)
void optimizeAssembly(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile);
    ofstream out(outputFile);

    if (!in.is_open() || !out.is_open()) {
        cerr << "Error opening files for optimization." << endl;
        return;
    }

    vector<string> lines;
    string line;
    while (getline(in, line)) {
        lines.push_back(line);
    }

    size_t i = 0;
    while (i < lines.size()) {
        string currentLine = lines[i];
        string trimmedCurrent = trim(currentLine);

        // Pattern 1: Redundant MOV (e.g., mov eax, eax OR mov ax, a followed by mov a, ax)
        if (trimmedCurrent.rfind("mov ", 0) == 0) {
            stringstream ss(trimmedCurrent.substr(4));
            string reg1, reg2;
            if (getline(ss, reg1, ',') && getline(ss, reg2)) {
                reg1 = trim(reg1);
                reg2 = trim(reg2);
                
                // Case A: mov eax, eax
                if (reg1 == reg2) {
                    i++; 
                    continue;
                }

                // Case B: mov ax, a followed by mov a, ax
                if (i + 1 < lines.size()) {
                    string nextLine = lines[i + 1];
                    string trimmedNext = trim(nextLine);
                    if (trimmedNext.rfind("mov ", 0) == 0) {
                        stringstream ss2(trimmedNext.substr(4));
                        string nReg1, nReg2;
                        if (getline(ss2, nReg1, ',') && getline(ss2, nReg2)) {
                            nReg1 = trim(nReg1);
                            nReg2 = trim(nReg2);
                            if (reg1 == nReg2 && reg2 == nReg1) {
                                out << currentLine << "\n"; // keep first MOV
                                i += 2; // skip second MOV
                                continue;
                            }
                        }
                    }
                }
            }
        }

        // Pattern 2: ADD / SUB with 0
        if (trimmedCurrent.rfind("add ", 0) == 0 || trimmedCurrent.rfind("sub ", 0) == 0) {
            stringstream ss(trimmedCurrent.substr(4));
            string reg, val;
            if (getline(ss, reg, ',') && getline(ss, val)) {
                val = trim(val);
                if (val == "0") {
                    i++; 
                    continue;
                }
            }
        }

        // Pattern 3: IMUL / MUL / IDIV / DIV by 1
        if (trimmedCurrent.rfind("imul ", 0) == 0 || trimmedCurrent.rfind("mul ", 0) == 0 ||
            trimmedCurrent.rfind("idiv ", 0) == 0 || trimmedCurrent.rfind("div ", 0) == 0) {
            stringstream ss(trimmedCurrent.substr(trimmedCurrent.find(' ') + 1));
            string reg, val;
            if (getline(ss, reg, ',') && getline(ss, val)) {
                val = trim(val);
                if (val == "1") {
                    i++; 
                    continue;
                }
            } else {
                // Single operand (e.g. mul bx where value is 1)
                reg = trim(ss.str());
                if (reg == "1") {
                    i++;
                    continue;
                }
            }
        }

        // Pattern 4: Redundant PUSH/POP Sequence
        if (i + 1 < lines.size()) {
            string nextLine = lines[i + 1];
            string trimmedNext = trim(nextLine);

            if (trimmedCurrent.rfind("push ", 0) == 0 && trimmedNext.rfind("pop ", 0) == 0) {
                string pushReg = trim(trimmedCurrent.substr(5));
                string popReg = trim(trimmedNext.substr(4));

                if (pushReg == popReg) {
                    i += 2; 
                    continue;
                }
            }
        }

        // Pattern 5: Redundant Consecutive Labels (e.g., L3: L4:)
        if (isLabel(trimmedCurrent) && i + 1 < lines.size()) {
            string nextLine = lines[i + 1];
            if (isLabel(nextLine)) {
                i++; // redundant label
                continue;
            }
        }

        
        out << currentLine << "\n";
        i++;
    }

    in.close();
    out.close();
    cout << "Optimization completed. Result saved to " << outputFile << endl;
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        cerr << "Error opening input file: " << argv[1] << endl;
        return 1;
    }


    ANTLRInputStream input(inputFile);
    CSubset_2205091Lexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CSubset_2205091Parser parser(&tokens);

    auto tree = parser.start(); 

    int bucketSize = 30;
    MyVisitor visitor(bucketSize, "code.asm");
    visitor.visit(tree);

    //Peephole Optimization
    optimizeAssembly("code.asm", "optimized_code.asm");

    inputFile.close();
    return 0;
}