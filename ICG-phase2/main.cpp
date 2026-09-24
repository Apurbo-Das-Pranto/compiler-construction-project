#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include "antlr4-runtime.h"
#include "CSubset_2205091Lexer.h"
#include "CSubset_2205091Parser.h"
#include "2205091_visitor.h"

using namespace antlr4;
using namespace std;

ofstream lexLogFile; 

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool isLabel(const string& line) {
    string t = trim(line);
    return !t.empty() && t.back() == ':';
}

static bool splitTwoOperands(const string& s, size_t opStart, string& dst, string& src) {
    size_t c = s.find(',', opStart);
    if (c == string::npos || opStart > s.size()) return false;
    dst = trim(s.substr(opStart, c - opStart));
    src = trim(s.substr(c + 1));
    return true;
}

static string destOf(const string& t) {
    static const char* twoOpWriters[] = {"mov ", "add ", "sub ", "and ", "or ", "xor ", "lea "};
    for (auto kw : twoOpWriters) {
        size_t len = strlen(kw);
        if (t.rfind(kw, 0) == 0) {
            size_t c = t.find(',');
            if (c != string::npos) return trim(t.substr(len, c - len));
        }
    }
    if (t.rfind("pop ", 0) == 0) return trim(t.substr(4));
    if (t.rfind("inc ", 0) == 0) return trim(t.substr(4));
    if (t.rfind("dec ", 0) == 0) return trim(t.substr(4));
    if (t.rfind("neg ", 0) == 0) return trim(t.substr(4));
    return "";
}

static bool isControlFlowOrCall(const string& t) {
    if (t.empty()) return true;
    if (t.back() == ':') return true;
    static const char* kws[] = {"jmp ", "je ", "jne ", "jl ", "jle ", "jg ", "jge ",
                                 "jz ", "jnz ", "call ", "loop"};
    for (auto kw : kws) if (t.rfind(kw, 0) == 0) return true;
    if (t == "ret") return true;
    return false;
}

static bool clobbersEaxEdx(const string& t) {
    return t.rfind("imul", 0) == 0 || t.rfind("mul", 0) == 0 ||
           t.rfind("idiv", 0) == 0 || t.rfind("div", 0) == 0 || t == "cdq";
}

void optimizeAssembly(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile);
    if (!in.is_open()) {
        cerr << "Error opening input file for optimization." << endl;
        return;
    }

    vector<string> lines;
    string line;
    while (getline(in, line)) {
        lines.push_back(line);
    }
    in.close();

    bool changed = true;
    while (changed) {
        changed = false;

        
        map<string, string> labelMap;
        for (size_t i = 0; i < lines.size(); i++) {
            string trimmedCurrent = trim(lines[i]);
            if (isLabel(trimmedCurrent)) {
                size_t j = i + 1;
                while (j < lines.size() && isLabel(trim(lines[j]))) {
                    j++;
                }
                if (j - i > 1) {
                    string keepLabel = trim(lines[i]);
                    keepLabel.pop_back();
                    for (size_t k = i + 1; k < j; k++) {
                        string deadLabel = trim(lines[k]);
                        deadLabel.pop_back();
                        labelMap[deadLabel] = keepLabel;
                    }
                    i = j - 1;
                }
            }
        }

        for (auto& entry : labelMap) {
            string current = entry.second;
            size_t guard = 0;
            while (labelMap.find(current) != labelMap.end() && guard++ < labelMap.size() + 1) {
                current = labelMap[current];
            }
            entry.second = current;
        }

        if (!labelMap.empty()) {
            vector<string> afterLabels;
            for (size_t i = 0; i < lines.size(); i++) {
                string trimmedCurrent = trim(lines[i]);
                if (isLabel(trimmedCurrent)) {
                    string lbl = trimmedCurrent;
                    lbl.pop_back();
                    if (labelMap.find(lbl) != labelMap.end()) {
                        changed = true;
                        continue;
                    }
                }

                string currentLine = lines[i];
                for (const auto& pair : labelMap) {
                    const string& deadLabel = pair.first;
                    const string& keepLabel = pair.second;
                    size_t pos = 0;
                    while ((pos = currentLine.find(deadLabel, pos)) != string::npos) {
                        bool leftBound = (pos == 0) ||
                            (!isalnum((unsigned char)currentLine[pos - 1]) && currentLine[pos - 1] != '_' && currentLine[pos - 1] != '.');
                        size_t endPos = pos + deadLabel.length();
                        bool rightBound = (endPos >= currentLine.length()) ||
                            (!isalnum((unsigned char)currentLine[endPos]) && currentLine[endPos] != '_');
                        if (leftBound && rightBound) {
                            currentLine.replace(pos, deadLabel.length(), keepLabel);
                            pos += keepLabel.length();
                            changed = true;
                        } else {
                            pos += deadLabel.length();
                        }
                    }
                }
                afterLabels.push_back(currentLine);
            }
            lines = afterLabels;
        }

        vector<string> nextLines;
        size_t i = 0;
        while (i < lines.size()) {
            string trimmedCurrent = trim(lines[i]);
            string trimmedNext = (i + 1 < lines.size()) ? trim(lines[i + 1]) : string();

            
            if (trimmedCurrent.rfind("mov ", 0) == 0) {
                string dst1, src1;
                if (splitTwoOperands(trimmedCurrent, 4, dst1, src1)) {
                    if (dst1 == src1) {
                        i++;
                        changed = true;
                        continue;
                    }
                    if (trimmedNext.rfind("mov ", 0) == 0) {
                        string dst2, src2;
                        if (splitTwoOperands(trimmedNext, 4, dst2, src2)) {
                            if (dst1 == src2 && src1 == dst2) {
                                nextLines.push_back(lines[i]);
                                i += 2;
                                changed = true;
                                continue;
                            }
                        }
                    }
                }
            }

            
            if (trimmedCurrent.rfind("push ", 0) == 0 && trimmedNext.rfind("pop ", 0) == 0) {
                string pushOperand = trim(trimmedCurrent.substr(5));
                string popOperand = trim(trimmedNext.substr(4));
                if (pushOperand == popOperand) {
                    i += 2;
                    changed = true;
                    continue;
                }
                if (pushOperand.find(' ') == string::npos && popOperand.find(' ') == string::npos) {
                    nextLines.push_back("    mov " + popOperand + ", " + pushOperand);
                    i += 2;
                    changed = true;
                    continue;
                }
            }

            
            if (trimmedCurrent.rfind("add ", 0) == 0 || trimmedCurrent.rfind("sub ", 0) == 0) {
                string reg, val;
                if (splitTwoOperands(trimmedCurrent, 4, reg, val) && val == "0") {
                    i++;
                    changed = true;
                    continue;
                }
            }

            
            if (trimmedCurrent.rfind("imul ", 0) == 0 || trimmedCurrent.rfind("mul ", 0) == 0 ||
                trimmedCurrent.rfind("idiv ", 0) == 0 || trimmedCurrent.rfind("div ", 0) == 0) {
                size_t sp = trimmedCurrent.find(' ');
                string reg, val;
                bool isIdentity = false;
                if (trimmedCurrent.find(',') != string::npos) {
                    if (splitTwoOperands(trimmedCurrent, sp + 1, reg, val) && val == "1") isIdentity = true;
                } else if (sp != string::npos) {
                    if (trim(trimmedCurrent.substr(sp + 1)) == "1") isIdentity = true;
                }
                if (isIdentity) {
                    i++;
                    changed = true;
                    continue;
                }
            }

            
            if (trimmedCurrent.rfind("mov ", 0) == 0 && !trimmedNext.empty()) {
                string reg, val;
                if (splitTwoOperands(trimmedCurrent, 4, reg, val)) {
                    if (val == "0" && (trimmedNext.rfind("add ", 0) == 0 || trimmedNext.rfind("sub ", 0) == 0)) {
                        string dst2, src2;
                        if (splitTwoOperands(trimmedNext, 4, dst2, src2) && src2 == reg) {
                            i += 2;
                            changed = true;
                            continue;
                        }
                    }
                    if (val == "1" && (trimmedNext.rfind("imul ", 0) == 0 || trimmedNext.rfind("mul ", 0) == 0)) {
                        size_t sp2 = trimmedNext.find(' ');
                        string tgt = (trimmedNext.find(',') != string::npos)
                                         ? [&]{ string d,s; splitTwoOperands(trimmedNext, sp2 + 1, d, s); return d; }()
                                         : trim(trimmedNext.substr(sp2 + 1));
                        if (tgt == reg) {
                            i += 2;
                            changed = true;
                            continue;
                        }
                    }
                }
            }

            nextLines.push_back(lines[i]);
            i++;
        }
        lines = nextLines;

        
        {
            vector<string> folded;
            const size_t WINDOW = 8;
            size_t i2 = 0;
            while (i2 < lines.size()) {
                string cur = trim(lines[i2]);
                bool didFold = false;

                if (cur.rfind("mov ", 0) == 0) {
                    string reg, val;
                    if (splitTwoOperands(cur, 4, reg, val) && (val == "0" || val == "1") &&
                        reg.find(' ') == string::npos && reg[0] != '[') {
                        size_t limit = min(lines.size(), i2 + 1 + WINDOW);
                        size_t j = i2 + 1;
                        while (j < limit) {
                            string t = trim(lines[j]);

                            if (val == "0" && (t.rfind("add ", 0) == 0 || t.rfind("sub ", 0) == 0)) {
                                string dst2, src2;
                                if (splitTwoOperands(t, 4, dst2, src2) && src2 == reg) {
                                    for (size_t k = i2 + 1; k < j; k++) folded.push_back(lines[k]);
                                    i2 = j + 1;
                                    didFold = true;
                                    changed = true;
                                }
                                break;
                            }
                            if (val == "1" && (t.rfind("imul ", 0) == 0 || t.rfind("mul ", 0) == 0) &&
                                t.find(',') == string::npos) {
                                string tgt = trim(t.substr(t.find(' ') + 1));
                                if (tgt == reg) {
                                    for (size_t k = i2 + 1; k < j; k++) folded.push_back(lines[k]);
                                    i2 = j + 1;
                                    didFold = true;
                                    changed = true;
                                }
                                break;
                            }
                            if (isControlFlowOrCall(t)) break;
                            if (clobbersEaxEdx(t) && (reg == "eax" || reg == "edx")) break;
                            string d = destOf(t);
                            if (!d.empty() && d == reg) break;
                            j++;
                        }
                    }
                }

                if (!didFold) {
                    folded.push_back(lines[i2]);
                    i2++;
                }
            }
            lines = folded;
        }
    }

    ofstream out(outputFile);
    if (!out.is_open()) {
        cerr << "Error opening output file for optimization." << endl;
        return;
    }
    for (const string& l : lines) {
        out << l << "\n";
    }
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

    
    optimizeAssembly("code.asm", "optimized_code.asm");

    inputFile.close();
    return 0;
}