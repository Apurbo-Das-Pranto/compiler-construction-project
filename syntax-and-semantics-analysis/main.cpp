#include <iostream>
#include <fstream>
#include <string>
#include "antlr4-runtime.h"
#include "CSubset_2205091Lexer.h"
#include "CSubset_2205091Parser.h"
#include "2205091_visitor.h"

using namespace antlr4;
using namespace std;

ofstream lexLogFile; 

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

    ofstream logFile("mylog.txt");
    ofstream errorFile("myerror.txt");

    streambuf *originalCout=cout.rdbuf();
    streambuf *originalCerr=cerr.rdbuf();

    cout.rdbuf(logFile.rdbuf());
    cerr.rdbuf(errorFile.rdbuf());

    ANTLRInputStream input(inputFile);
    CSubset_2205091Lexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CSubset_2205091Parser parser(&tokens);

    CSubset_2205091Parser::StartContext* tree = parser.start();
    int bucketSize=30;
    MyVisitor visitor(bucketSize);
    visitor.visit(tree);

    // cout << "Parsing completed." << endl;

    cout.rdbuf(originalCout);
    cerr.rdbuf(originalCerr);
    inputFile.close();
    logFile.close();
    errorFile.close();
    return 0;
}
