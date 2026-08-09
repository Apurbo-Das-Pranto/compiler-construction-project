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

string currentLexeme = "";
string currentDecoded = "";
int startLine = 1;

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

/* Exclusive Start States for String and Comments */
%x STRING_STATE
%x SINGLE_LINE_COMMENT
%x MULTI_LINE_COMMENT

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
CHAR_CONST      {SINGLE_QUOTE}([^'\\\n]|\\['"\\ntafrbv0]){SINGLE_QUOTE}

%%

{WHITESPACE}    ;

\n              { lineCount++; }

    /* 1. STRING STATE (Single & Multi-line String)  */
"\"" {
    BEGIN(STRING_STATE);
    currentLexeme = "\"";
    currentDecoded = "";
    startLine = lineCount;
}

<STRING_STATE>\" {
    currentLexeme += "\"";
    BEGIN(INITIAL);
    writeToken("STRING", currentDecoded);
    /* print last line (lineCount) */
    logOutput << "Line no " << lineCount << ": Token <STRING> Lexeme " << currentLexeme << " found --> <STRING, " << currentDecoded << ">\n\n";
}

<STRING_STATE>\\\n {
    /* Line Continuation in Multi-line String */
    currentLexeme += "\\\n";
    lineCount++;
}

<STRING_STATE>\\n       { currentLexeme += "\\n"; currentDecoded += '\n'; }
<STRING_STATE>\\t       { currentLexeme += "\\t"; currentDecoded += '\t'; }
<STRING_STATE>\\r       { currentLexeme += "\\r"; currentDecoded += '\r'; }
<STRING_STATE>\\a       { currentLexeme += "\\a"; currentDecoded += '\a'; }
<STRING_STATE>\\b       { currentLexeme += "\\b"; currentDecoded += '\b'; }
<STRING_STATE>\\f       { currentLexeme += "\\f"; currentDecoded += '\f'; }
<STRING_STATE>\\v       { currentLexeme += "\\v"; currentDecoded += '\v'; }
<STRING_STATE>\\0       { currentLexeme += "\\0"; currentDecoded += '\0'; }
<STRING_STATE>\\\\      { currentLexeme += "\\\\"; currentDecoded += '\\'; }
<STRING_STATE>\\\"      { currentLexeme += "\\\""; currentDecoded += '"'; }
<STRING_STATE>\\\'      { currentLexeme += "\\'"; currentDecoded += '\''; }
<STRING_STATE>\\.       { currentLexeme += yytext; currentDecoded += yytext[1]; }

<STRING_STATE>\n {
    /* Unescaped Newline inside String -> Unterminated String Error (starting line of the string) */
    BEGIN(INITIAL);
    logOutput << "Error at line no " << startLine << ": Unterminated string " << currentLexeme << "\n\n";
    errorCount++;
    lineCount++;
}

<STRING_STATE><<EOF>> {
    BEGIN(INITIAL);
    logOutput << "Error at line no " << startLine << ": Unterminated string " << currentLexeme << "\n\n";
    errorCount++;
}

<STRING_STATE>. {
    currentLexeme += yytext;
    currentDecoded += yytext;
}


    /* 2. SINGLE LINE COMMENT STATE */
"//" {
    BEGIN(SINGLE_LINE_COMMENT);
    currentLexeme = "//";
    startLine = lineCount;
}

<SINGLE_LINE_COMMENT>\\\n {
    /* Line continuation in single line comment */
    currentLexeme += "\\\n";
    lineCount++;
}

<SINGLE_LINE_COMMENT>\n {
    BEGIN(INITIAL);
    logOutput << "Line no " << lineCount << ": Token <COMMENT> Lexeme " << currentLexeme << " found\n\n";
    lineCount++;
}

<SINGLE_LINE_COMMENT><<EOF>> {
    BEGIN(INITIAL);
    logOutput << "Line no " << lineCount << ": Token <COMMENT> Lexeme " << currentLexeme << " found\n\n";
}

<SINGLE_LINE_COMMENT>. {
    currentLexeme += yytext;
}


    /* 3. MULTI LINE COMMENT STATE */

"/*" {
    BEGIN(MULTI_LINE_COMMENT);
    currentLexeme = "/*";
    startLine = lineCount;
}

<MULTI_LINE_COMMENT>"*/" {
    currentLexeme += "*/";
    BEGIN(INITIAL);
    logOutput << "Line no " << lineCount << ": Token <COMMENT> Lexeme " << currentLexeme << " found\n\n";
}

<MULTI_LINE_COMMENT>\n {
    currentLexeme += "\n";
    lineCount++;
}

<MULTI_LINE_COMMENT><<EOF>> {
    BEGIN(INITIAL);
    logOutput << "Error at line no " << startLine << ": Unterminated comment " << currentLexeme << "\n\n";
    errorCount++;
}

<MULTI_LINE_COMMENT>. {
    currentLexeme += yytext;
}


    /* 4. KEYWORDS, IDENTIFIERS & CONSTANTS */
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


    /* 5. OPERATORS AND PUNCTUATORS */
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


    /* 6. ERROR HANDLING */
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