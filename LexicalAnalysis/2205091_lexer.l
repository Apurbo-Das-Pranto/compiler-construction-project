%option noyywrap nounput noinput

%{
#include <bits/stdc++.h>
#include "2205091_symbol_table.hpp"
using namespace std;

int lineCount = 1;
int errorCount = 0;
ofstream tokenOutput;
ofstream logOutput;
SymbolTable *symbolTable = nullptr;

string to_upper(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

void writeToken(const string &tokenType, const string &tokenValue, bool keyword = false) {
    if (keyword) {
        tokenOutput << "<" << tokenType << "> ";
    } else {
        tokenOutput << "<" << tokenType << ", " << tokenValue << "> ";
    }
}

void logSimpleToken(const string &tokenType, const string &lexeme, const string &displayValue = "") {
    if (tokenType == "CONST_CHAR") {
        logOutput << "Line no " << lineCount << ": Token <" << tokenType << "> Lexeme " << lexeme << " found --> <CONST_CHAR, " << displayValue << ">\n\n";
    } else if (tokenType == "STRING") {
        logOutput << "Line no " << lineCount << ": Token <" << tokenType << "> Lexeme " << lexeme << " found --> <STRING, " << displayValue << ">\n\n";
    } else {
        logOutput << "Line no " << lineCount << ": Token <" << tokenType << "> Lexeme " << lexeme << " found\n\n";
    }
}

string decodeCharLiteral(const string &lexeme) {
    if (lexeme.size() == 3) {
        return string(1, lexeme[1]);
    }
    if (lexeme.size() == 4 && lexeme[0] == '\'' && lexeme[1] == '\\') {
        switch (lexeme[2]) {
            case 'n': return "\n";
            case 't': return "\t";
            case 'r': return "\r";
            case 'a': return "\a";
            case 'b': return "\b";
            case 'f': return "\f";
            case 'v': return "\v";
            case '0': return "\0";
            case '\\': return "\\";
            case '\'': return "'";
            default: return string(1, lexeme[2]);
        }
    }
    return lexeme;
}

string decodeStringLiteral(const string &lexeme) {
    string result;
    for (size_t i = 1; i + 1 < lexeme.size(); ++i) {
        char c = lexeme[i];
        if (c == '\\' && i + 1 < lexeme.size()) {
            char next = lexeme[i + 1];
            if (next == '\n') {
                i++; // skip the escaped newline line continuation
                continue;
            }
            if (next == 'n') {
                result.push_back('\n');
                i++;
                continue;
            }
            if (next == 't') {
                result.push_back('\t');
                i++;
                continue;
            }
            if (next == 'r') {
                result.push_back('\r');
                i++;
                continue;
            }
            if (next == '\\') {
                result.push_back('\\');
                i++;
                continue;
            }
            if (next == '"') {
                result.push_back('"');
                i++;
                continue;
            }
            result.push_back(next);
            i++;
            continue;
        }
        result.push_back(c);
    }
    return result;
}

void emitError(const string &message, bool extraBlankLine=false) {
    logOutput << "Error at line no " << lineCount << ": " << message << "\n\n";
    if (extraBlankLine) {
        logOutput << "\n";
    }
    errorCount++;
}

bool insertSymbol(const string &name, const string &type) {
    if (symbolTable != nullptr) {
        bool ok = symbolTable->insertSymbolInfo(name, type);
        if (ok) {
            symbolTable->printAllScopeTable();
        }
        return ok;
    }
    return false;
}

%}

WHITESPACE      [ \t\f\r\v]
ALPHABET        [a-zA-Z]
DIGIT           [0-9]
UNDER_SCORE     _
KEYWORD         if|else|for|while|do|break|continue|return|int|float|double|char|long|short|unsigned|void|static|switch|case|default|goto
ADDOP           [+-]
MULOP           [*/%]
INCOP           "++"|"--"
RELOP           "<="|">="|"=="|"!="|"<"|">"
ASSIGNOP        "="
LOGICOP         "&&"|"||"
NOT             "!"
LPAREN          "("
RPAREN          ")"
LCURL           "{"
RCURL           "}"
LTHIRD          "["
RTHIRD          "]"
COMMA           ","
SEMICOLON       ";"
ALPHANUMERIC    ({ALPHABET}|{DIGIT})
IDENTIFIER      ({ALPHABET}|{UNDER_SCORE})({ALPHANUMERIC}|{UNDER_SCORE})*
EXPONENT        [Ee][+-]?{DIGIT}+
CONST_INT       {DIGIT}+
CONST_FLOAT     ({DIGIT}+\.{DIGIT}*|\.{DIGIT}+){EXPONENT}?|{DIGIT}+{EXPONENT}
SINGLE_QUOTE    "'"
DOUBLE_QUOTE    "\""
CHAR_CONST      {SINGLE_QUOTE}([^'\\\n]|\\['"\\ntafrbv0]){SINGLE_QUOTE}
STRING_CONST    {DOUBLE_QUOTE}([^"\\\n]|\\.|\\\n)*{DOUBLE_QUOTE}

%%

{WHITESPACE}    ;

\n              { lineCount++; }

{KEYWORD}       {
                    string lex = yytext;
                    string token = to_upper(lex);
                    writeToken(token, lex, true);
                    logSimpleToken(token, lex);
                }

{IDENTIFIER}    {
                    string lex = yytext;
                    writeToken("ID", lex);
                    logSimpleToken("ID", lex);
                    insertSymbol(lex, "ID");
                }

{CONST_FLOAT}   {
                    string lex = yytext;
                    writeToken("CONST_FLOAT", lex);
                    logSimpleToken("CONST_FLOAT", lex);
                    insertSymbol(lex, "CONST_FLOAT");
                }

{CONST_INT}     {
                    string lex = yytext;
                    writeToken("CONST_INT", lex);
                    logSimpleToken("CONST_INT", lex);
                    insertSymbol(lex, "CONST_INT");
                }

{CHAR_CONST}    {
                    string lex = yytext;
                    string value = decodeCharLiteral(lex);
                    writeToken("CONST_CHAR", value);
                    logSimpleToken("CONST_CHAR", lex, value);
                    insertSymbol(lex, "CONST_CHAR");
                }

{STRING_CONST}  {
                    string lex = yytext;
                    string value = decodeStringLiteral(lex);
                    int newlineCount = 0;
                    for (char c : lex) if (c == '\n') newlineCount++;
                    lineCount += newlineCount;
                    writeToken("STRING", value);
                    logSimpleToken("STRING", lex, value);
                }

{ADDOP}         { writeToken("ADDOP", yytext); logSimpleToken("ADDOP", yytext); }
{MULOP}         { writeToken("MULOP", yytext); logSimpleToken("MULOP", yytext); }
{INCOP}         { writeToken("INCOP", yytext); logSimpleToken("INCOP", yytext); }
{RELOP}         { writeToken("RELOP", yytext); logSimpleToken("RELOP", yytext); }
{ASSIGNOP}      { writeToken("ASSIGNOP", yytext); logSimpleToken("ASSIGNOP", yytext); }
{LOGICOP}       { writeToken("LOGICOP", yytext); logSimpleToken("LOGICOP", yytext); }
{NOT}           { writeToken("NOT", yytext); logSimpleToken("NOT", yytext); }
{LPAREN}        { writeToken("LPAREN", yytext); logSimpleToken("LPAREN", yytext); }
{RPAREN}        { writeToken("RPAREN", yytext); logSimpleToken("RPAREN", yytext); }
{LCURL}         { writeToken("LCURL", yytext); logSimpleToken("LCURL", yytext); symbolTable->enterScope(); }
{RCURL}         { writeToken("RCURL", yytext); logSimpleToken("RCURL", yytext); symbolTable->exitScope(true); }
{LTHIRD}        { writeToken("LTHIRD", yytext); logSimpleToken("LTHIRD", yytext); }
{RTHIRD}        { writeToken("RTHIRD", yytext); logSimpleToken("RTHIRD", yytext); }
{COMMA}         { writeToken("COMMA", yytext); logSimpleToken("COMMA", yytext); }
{SEMICOLON}     { writeToken("SEMICOLON", yytext); logSimpleToken("SEMICOLON", yytext); }

{DIGIT}+[_]?[a-zA-Z]+ {
                    emitError("Invalid prefix on ID or invalid suffix on Number " + string(yytext));
                }

{DIGIT}*\.+[0-9\.]* {
                    emitError("Too many decimal points " + string(yytext), true);
                }

({DIGIT}*[eE][+-]?{CONST_FLOAT}+)|({DIGIT}*\.+[0-9\.eE+-]*) {
                    emitError("Ill formed number " + string(yytext), true);
                }

{SINGLE_QUOTE}{SINGLE_QUOTE} {
                    emitError("Empty character constant error " + string(yytext), true);
                }

{SINGLE_QUOTE}[^'\n]{2,255}{SINGLE_QUOTE} {
                    emitError("Multi character constant error " + string(yytext), true);
                }

{SINGLE_QUOTE}\\{SINGLE_QUOTE}[^'\n]*\n {
                    emitError("Unterminated character " + string(yytext));
                    lineCount++;
                }

{SINGLE_QUOTE}[^'\n]*\n {
                    emitError("Unterminated character " + string(yytext));
                    lineCount++;
                }

{SINGLE_QUOTE} {
                    emitError("Unterminated character " + string(yytext), true);
                }

{DOUBLE_QUOTE}([^"\\\n]|\\.|\\\n)* {
                    string lex = yytext;
                    int newlineCount = 0;
                    for (char c : lex) if (c == '\n') newlineCount++;
                    emitError("Unterminated string " + lex);
                    lineCount += newlineCount;
                }

"//"[^\n]*(\\\n[^\n]*)* {
                    string lex = yytext;
                    int newlineCount = 0;
                    for (char c : lex) if (c == '\n') newlineCount++;
                    logOutput << "Line no " << lineCount + newlineCount << ": Token <COMMENT> Lexeme " << lex << " found\n\n";
                    lineCount += newlineCount;
                }

"/*"([^*]|"*"[^*/])*"*"+"/" {
                    string lex = yytext;
                    int newlineCount = 0;
                    for (char c : lex) if (c == '\n') newlineCount++;
                    logOutput << "Line no " << lineCount + newlineCount << ": Token <COMMENT> Lexeme " << lex << " found\n\n";
                    lineCount += newlineCount;
                }

"/*"([^*]|"*"[^*/])*     {
                    string lex = yytext;
                    int newlineCount = 0;
                    for (char c : lex) if (c == '\n') newlineCount++;
                    emitError("Unterminated comment " + lex);
                    lineCount += newlineCount;
                }

.               {
                    emitError("Unrecognized character " + string(yytext));
                }

<<EOF>>        {
                    yyterminate();
                }

%%

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "please provide input file, token output file and log output file name as command line argument" << endl;
        return 0;
    }


    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        cerr << "Error: Could not open input file." << endl;
        return 1;
    }

    tokenOutput.open(argv[2]);
    if (!tokenOutput.is_open()) {
        cerr << "Error: Could not open token output file." << endl;
        return 1;
    }

    logOutput.open(argv[3]);
    if (!logOutput.is_open()) {
        cerr << "Error: Could not open log output file." << endl;
        return 1;
    }

    streambuf *originalCoutBuffer = cout.rdbuf();
    cout.rdbuf(logOutput.rdbuf());

    yyin = inputFile;
    symbolTable = new SymbolTable(7);

    yylex();

    symbolTable->printAllScopeTable();
    logOutput << "Total lines: " << lineCount << endl;
    logOutput << "Total errors: " << errorCount << endl;

    cout.rdbuf(originalCoutBuffer);

    fclose(inputFile);
    tokenOutput.close();
    logOutput.close();
    delete symbolTable;

    return 0;
}