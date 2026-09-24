#include "2205091_visitor.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

MyVisitor::MyVisitor(int bucketSize)
{
    symbolTable = new SymbolTable(bucketSize);
    errorCount = 0;
    currentType = "";
}


// HELPERS

// Strip trailing error tokens (+=, -=) from expressions
string cleanExprText(string text) {
    while (text.length() >= 2 && (text.substr(text.length() - 2) == "+=" || text.substr(text.length() - 2) == "-=")) {
        text = text.substr(0, text.length() - 2);
    }
    return text;
}

void MyVisitor::logMatch(antlr4::ParserRuleContext *ctx, const string &production)
{
    logMatch(ctx, production, ctx->getStart()->getLine());
}

void MyVisitor::logMatch(antlr4::ParserRuleContext *ctx, const string &production, int line)
{
    cout << "Line " << line << ": " << production << "\n\n"
         << ctx->getText() << "\n\n";
}

void MyVisitor::logMatch(antlr4::ParserRuleContext *ctx, const string &production, const string &text)
{
    logMatch(ctx, production, text, ctx->getStart()->getLine());
}

void MyVisitor::logMatch(antlr4::ParserRuleContext *ctx, const string &production, const string &text, int line)
{
    cout << "Line " << line << ": " << production << "\n\n"
         << text << "\n\n";
}

void MyVisitor::reportError(antlr4::ParserRuleContext *ctx, const string &message)
{
    reportError(ctx->getStart()->getLine(), message);
}

void MyVisitor::reportError(int line, const string &message)
{
    errorCount++;
    cout << "Error at line " << line << ": " << message << endl
         << endl;
    cerr << "Error at line " << line << ": " << message << endl
         << endl;
}


// PROGRAM & UNITS

any MyVisitor::visitStartRule(CSubset_2205091Parser::StartRuleContext *ctx)
{
    visit(ctx->program());
    cout << "Line " << ctx->getStart()->getLine() << ": start : program" << endl;
    symbolTable->printAllScopeTable();
    cout << "Total lines: " << ctx->getStop()->getLine() << endl;
    cout << "Total errors: " << errorCount << endl << endl;
    return nullptr;
}

any MyVisitor::visitProgramSingleUnit(CSubset_2205091Parser::ProgramSingleUnitContext *ctx)
{
    string unitText = any_cast<string>(visit(ctx->unit()));
    logMatch(ctx, "program : unit", unitText);
    cout << "\n";
    return unitText;
}

any MyVisitor::visitProgramUnits(CSubset_2205091Parser::ProgramUnitsContext *ctx)
{
    string progText = any_cast<string>(visit(ctx->program()));
    string unitText = any_cast<string>(visit(ctx->unit()));

    string fullText = progText + "\n" + unitText;
    logMatch(ctx, "program : program unit", fullText, ctx->unit()->getStart()->getLine());
    cout << "\n";
    return fullText;
}

any MyVisitor::visitUnitVarDecl(CSubset_2205091Parser::UnitVarDeclContext *ctx)
{
    string text = any_cast<string>(visit(ctx->var_declaration()));

    logMatch(ctx, "unit : var_declaration", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitUnitFuncDecl(CSubset_2205091Parser::UnitFuncDeclContext *ctx)
{
    string text = any_cast<string>(visit(ctx->func_declaration()));

    logMatch(ctx, "unit : func_declaration", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitUnitFuncDef(CSubset_2205091Parser::UnitFuncDefContext *ctx)
{
    string text = any_cast<string>(visit(ctx->func_definition()));
    text += "\n";
    logMatch(ctx, "unit : func_definition", text);
    cout << "\n";
    return text;
}

// FUNCTION DECLARATIONS & DEFINITIONS

any MyVisitor::visitFuncDeclWithParams(CSubset_2205091Parser::FuncDeclWithParamsContext *ctx)
{
    string retType = any_cast<string>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    auto params = any_cast<vector<Parameter>>(visit(ctx->parameter_list()));

    SymbolInfo *existing = symbolTable->lookUpCurrentScope(name);
    if (existing != nullptr)
    {
        reportError(ctx, "Multiple declaration of " + name);
    }
    else
    {
        SymbolInfo *funcSymbol = new SymbolInfo(name, "ID");
        funcSymbol->setDataType(retType);
        funcSymbol->setIsFunction(true);
        funcSymbol->setIsDefined(false);
        for (auto &p : params)
        {
            funcSymbol->addParameter(p.name, p.type);
        }
        symbolTable->insertSymbolInfo(funcSymbol);
    }

    symbolTable->enterScope();
    symbolTable->exitScope(false);

    string paramStr = "";
    for (size_t i = 0; i < params.size(); i++)
    {
        if (i > 0)
            paramStr += ",";
        string lowerType = params[i].type;
        transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
        paramStr += lowerType;
        if (!params[i].name.empty())
            paramStr += " " + params[i].name;
    }

    string lowerReturntype = retType;
    transform(lowerReturntype.begin(), lowerReturntype.end(), lowerReturntype.begin(), ::tolower);
    string text = lowerReturntype + " " + name + "(" + paramStr + ");";

    logMatch(ctx, "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitFuncDeclNoParams(CSubset_2205091Parser::FuncDeclNoParamsContext *ctx)
{
    string retType = any_cast<string>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();

    SymbolInfo *existing = symbolTable->lookUpCurrentScope(name);
    if (existing != nullptr)
    {
        reportError(ctx, "Multiple declaration of " + name);
    }
    else
    {
        SymbolInfo *funcSymbol = new SymbolInfo(name, "ID");
        funcSymbol->setDataType(retType);
        funcSymbol->setIsFunction(true);
        funcSymbol->setIsDefined(false);
        symbolTable->insertSymbolInfo(funcSymbol);
    }

    symbolTable->enterScope();
    symbolTable->exitScope(false);

    string lowerReturntype = retType;
    transform(lowerReturntype.begin(), lowerReturntype.end(), lowerReturntype.begin(), ::tolower);
    string text = lowerReturntype + " " + name + "();";

    logMatch(ctx, "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitFuncDefWithParams(CSubset_2205091Parser::FuncDefWithParamsContext *ctx)
{
    string retType = any_cast<string>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();
    auto params = any_cast<vector<Parameter>>(visit(ctx->parameter_list()));

    SymbolInfo *existing = symbolTable->lookUpSymbolInfo(name);
    if (existing != nullptr)
    {
        if (!existing->getIsFunction())
        {
            reportError(ctx, "Multiple declaration of " + name);
        }
        else if (existing->getIsDefined())
        {
            reportError(ctx, "Multiple definition of " + name);
        }
        else if (existing->getDataType() != retType)
        {
            reportError(ctx, "Return type mismatch with function declaration in function " + name);
        }
        if (existing->getIsFunction() && existing->getParamList().size() != params.size())
        {
            reportError(ctx, "Total number of arguments mismatch with declaration in function " + name);
        }
        existing->setIsDefined(true);
    }
    else
    {
        SymbolInfo *funcSymbol = new SymbolInfo(name, "ID");
        funcSymbol->setDataType(retType);
        funcSymbol->setIsFunction(true);
        funcSymbol->setIsDefined(true);
        for (auto &p : params)
        {
            funcSymbol->addParameter(p.name, p.type);
        }
        symbolTable->insertSymbolInfo(funcSymbol);
    }

    symbolTable->enterScope();

    for (size_t i = 0; i < params.size(); i++)
    {
        auto &p = params[i];
        if (p.name.empty())
        {
            reportError(ctx, to_string(i + 1) + "th parameter's name not given in function definition of " + name);
            continue;
        }
        SymbolInfo *pSymbol = symbolTable->insertSymbolInfo(p.name, "ID");
        if (pSymbol != nullptr)
        {
            pSymbol->setDataType(p.type);
        }
    }

    string compoundText = any_cast<string>(visit(ctx->compound_statement()));
    symbolTable->exitScope();

    string paramStr = "";
    for (size_t i = 0; i < params.size(); i++)
    {
        if (i > 0)
            paramStr += ",";
        string lowerType = params[i].type;
        transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
        paramStr += lowerType;
        if (!params[i].name.empty()) {
            paramStr += " " + params[i].name;
        }
    }

    string lowerReturntype = retType;
    transform(lowerReturntype.begin(), lowerReturntype.end(), lowerReturntype.begin(), ::tolower);
    string funcDefText = lowerReturntype + " " + name + "(" + paramStr + ")" + compoundText;

    logMatch(ctx, "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement", funcDefText);
    cout << "\n";
    return funcDefText;
}

any MyVisitor::visitFuncDefNoParams(CSubset_2205091Parser::FuncDefNoParamsContext *ctx)
{
    string retType = any_cast<string>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();

    SymbolInfo *existing = symbolTable->lookUpSymbolInfo(name);
    if (existing != nullptr)
    {
        if (!existing->getIsFunction())
        {
            reportError(ctx, "Multiple declaration of " + name);
        }
        else if (existing->getIsDefined())
        {
            reportError(ctx, "Multiple definition of " + name);
        }
        else if (existing->getDataType() != retType)
        {
            reportError(ctx, "Return type mismatch with function declaration in function " + name);
        }
        else if (!existing->getParamList().empty())
        {
            reportError(ctx, "Total number of arguments mismatch with declaration in function " + name);
        }
        if (existing->getIsFunction())
            existing->setIsDefined(true);
    }
    else
    {
        SymbolInfo *funcSymbol = new SymbolInfo(name, "ID");
        funcSymbol->setDataType(retType);
        funcSymbol->setIsFunction(true);
        funcSymbol->setIsDefined(true);
        symbolTable->insertSymbolInfo(funcSymbol);
    }

    symbolTable->enterScope();
    string compoundText = any_cast<string>(visit(ctx->compound_statement()));
    symbolTable->exitScope();

    string lowerReturntype = retType;
    transform(lowerReturntype.begin(), lowerReturntype.end(), lowerReturntype.begin(), ::tolower);
    string funcDefText = lowerReturntype + " " + name + "()" + compoundText;

    logMatch(ctx, "func_definition : type_specifier ID LPAREN RPAREN compound_statement", funcDefText);
    cout << "\n";
    return funcDefText;
}

// PARAMETER LISTS

any MyVisitor::visitParamListMulti(CSubset_2205091Parser::ParamListMultiContext *ctx)
{
    auto params = any_cast<vector<Parameter>>(visit(ctx->parameter_list()));
    string type = any_cast<string>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();

    // Check for duplicate parameter names
    bool isDuplicate = false;
    for (const auto &p : params)
    {
        if (!name.empty() && p.name == name)
        {
            isDuplicate = true;
            break;
        }
    }

    if (isDuplicate)
    {
        reportError(ctx, "Multiple declaration of " + name + " in parameter");
    }

    params.push_back({name, type});

    string paramText = "";
    for (size_t i = 0; i < params.size(); i++)
    {
        if (i > 0)
            paramText += ",";
        string lowerType = params[i].type;
        transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
        paramText += lowerType + " " + params[i].name;
    }

    logMatch(ctx, "parameter_list : parameter_list COMMA type_specifier ID", paramText);
    return params;
}

any MyVisitor::visitParamListMultiUnnamed(CSubset_2205091Parser::ParamListMultiUnnamedContext *ctx)
{
    auto params = any_cast<vector<Parameter>>(visit(ctx->parameter_list()));
    string type = any_cast<string>(visit(ctx->type_specifier()));
    params.push_back({"", type});
    logMatch(ctx, "parameter_list : parameter_list COMMA type_specifier");
    return params;
}

any MyVisitor::visitParamSingleUnnamed(CSubset_2205091Parser::ParamSingleUnnamedContext *ctx)
{
    string type = any_cast<string>(visit(ctx->type_specifier()));
    logMatch(ctx, "parameter_list : type_specifier");
    return vector<Parameter>{{"", type}};
}

any MyVisitor::visitParamSingle(CSubset_2205091Parser::ParamSingleContext *ctx)
{
    string type = any_cast<string>(visit(ctx->type_specifier()));
    string name = ctx->ID()->getText();

    string lowerType = type;
    transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    string paramText = lowerType + " " + name;

    logMatch(ctx, "parameter_list : type_specifier ID", paramText);
    return vector<Parameter>{{name, type}};
}

any MyVisitor::visitParamADDOPError(CSubset_2205091Parser::ParamADDOPErrorContext *ctx)
{
    string type = any_cast<string>(visit(ctx->type_specifier()));
    string lowerType = type;
    transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    
    logMatch(ctx, "parameter_list : type_specifier", lowerType);

    string op = ctx->ADDOP()->getText();
    reportError(ctx, "syntax error, unexpected token(s) '" + op + "' before ')'");

    return vector<Parameter>{{"", type}};
}

// COMPOUND STATEMENTS

any MyVisitor::visitCompoundStmtWithStmts(CSubset_2205091Parser::CompoundStmtWithStmtsContext *ctx)
{
    string stmtsText = any_cast<string>(visit(ctx->statements()));
    string compoundText = "{\n" + stmtsText + "\n}";

    logMatch(ctx, "compound_statement : LCURL statements RCURL", compoundText);
    return compoundText;
}

any MyVisitor::visitCompoundStmtEmpty(CSubset_2205091Parser::CompoundStmtEmptyContext *ctx)
{
    string compoundText = "{}";
    logMatch(ctx, "compound_statement : LCURL RCURL", compoundText);
    return compoundText;
}

// TYPE SPECIFIERS

any MyVisitor::visitTypeInt(CSubset_2205091Parser::TypeIntContext *ctx)
{
    logMatch(ctx, "type_specifier : INT");
    return string("INT");
}

any MyVisitor::visitTypeFloat(CSubset_2205091Parser::TypeFloatContext *ctx)
{
    logMatch(ctx, "type_specifier : FLOAT");
    return string("FLOAT");
}

any MyVisitor::visitTypeVoid(CSubset_2205091Parser::TypeVoidContext *ctx)
{
    logMatch(ctx, "type_specifier : VOID");
    return string("VOID");
}

// DECLARATION LISTS & VARIABLES

any MyVisitor::visitVarDeclaration(CSubset_2205091Parser::VarDeclarationContext *ctx)
{
    string type = any_cast<string>(visit(ctx->type_specifier()));
    this->currentType = type;

    string declListText = any_cast<string>(visit(ctx->declaration_list()));

    string lowerType = type;
    transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);

    string text = lowerType + " " + declListText + ";";

    logMatch(ctx, "var_declaration : type_specifier declaration_list SEMICOLON", text);
    return text;
}

any MyVisitor::visitDeclVar(CSubset_2205091Parser::DeclVarContext *ctx)
{
    string name = ctx->ID()->getText();

    if (currentType != "VOID")
    {
        SymbolInfo *s = new SymbolInfo(name, "ID");
        s->setDataType(currentType);
        SymbolInfo *inserted = symbolTable->insertSymbolInfo(s);
        if (inserted == nullptr)
        {
            reportError(ctx, "Multiple declaration of " + name);
            delete s;
        }
    }

    logMatch(ctx, "declaration_list : ID");

    if (currentType == "VOID")
    {
        reportError(ctx, "Variable type cannot be void");
    }

    return name;
}

any MyVisitor::visitDeclArray(CSubset_2205091Parser::DeclArrayContext *ctx)
{
    string name = ctx->ID()->getText();
    string sz = ctx->CONST_INT()->getText();

    if (currentType != "VOID")
    {
        SymbolInfo *symbol = new SymbolInfo(name, "ID");
        symbol->setIsArray(true);
        symbol->setArraySize(stoi(sz));
        symbol->setDataType(currentType);
        SymbolInfo *inserted = symbolTable->insertSymbolInfo(symbol);
        if (inserted == nullptr)
        {
            reportError(ctx, "Multiple declaration of " + name);
            delete symbol;
        }
    }

    logMatch(ctx, "declaration_list : ID LTHIRD CONST_INT RTHIRD");

    if (currentType == "VOID")
    {
        reportError(ctx, "Variable type cannot be void");
    }

    return name + "[" + sz + "]";
}

any MyVisitor::visitDeclListVar(CSubset_2205091Parser::DeclListVarContext *ctx)
{
    string listText = any_cast<string>(visit(ctx->declaration_list()));
    string name = ctx->ID()->getText();

    if (currentType != "VOID")
    {
        SymbolInfo *s = new SymbolInfo(name, "ID");
        s->setDataType(currentType);
        SymbolInfo *inserted = symbolTable->insertSymbolInfo(s);
        if (inserted == nullptr)
        {
            reportError(ctx, "Multiple declaration of " + name);
            delete s;
        }
    }

    string fullText = listText + "," + name;
    logMatch(ctx, "declaration_list : declaration_list COMMA ID", fullText);

    if (currentType == "VOID")
    {
        reportError(ctx, "Variable type cannot be void");
    }

    return fullText;
}

any MyVisitor::visitDeclListArray(CSubset_2205091Parser::DeclListArrayContext *ctx)
{
    string listText = any_cast<string>(visit(ctx->declaration_list()));
    string name = ctx->ID()->getText();
    string sz = ctx->CONST_INT()->getText();

    if (currentType != "VOID")
    {
        SymbolInfo *s = new SymbolInfo(name, "ID");
        s->setIsArray(true);
        s->setArraySize(stoi(sz));
        s->setDataType(currentType);
        SymbolInfo *inserted = symbolTable->insertSymbolInfo(s);
        if (inserted == nullptr)
        {
            reportError(ctx, "Multiple declaration of " + name);
            delete s;
        }
    }

    string fullText = listText + "," + name + "[" + sz + "]";
    logMatch(ctx, "declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD", fullText);

    if (currentType == "VOID")
    {
        reportError(ctx, "Variable type cannot be void");
    }

    return fullText;
}

any MyVisitor::visitDeclADDOPError(CSubset_2205091Parser::DeclADDOPErrorContext *ctx)
{
    string listText = any_cast<string>(visit(ctx->declaration_list()));

    string op = ctx->ADDOP()->getText();
    string varName = ctx->ID()->getText();
    reportError(ctx, "syntax error, unexpected token(s) '" + op + " " + varName + "' in declaration list");

    return listText; 
}


// STATEMENTS

any MyVisitor::visitSingleStmt(CSubset_2205091Parser::SingleStmtContext *ctx)
{
    string stmtText = any_cast<string>(visit(ctx->statement()));
    logMatch(ctx, "statements : statement", stmtText);
    cout << "\n";
    return stmtText;
}

any MyVisitor::visitMultipleStmts(CSubset_2205091Parser::MultipleStmtsContext *ctx)
{
    string stmtsText = any_cast<string>(visit(ctx->statements()));
    string stmtText = any_cast<string>(visit(ctx->statement()));

    string fullText = stmtsText + "\n" + stmtText;
    logMatch(ctx, "statements : statements statement", fullText, ctx->statement()->getStart()->getLine());
    cout << "\n";
    return fullText;
}

any MyVisitor::visitStmtVarDecl(CSubset_2205091Parser::StmtVarDeclContext *ctx)
{
    string text = any_cast<string>(visit(ctx->var_declaration()));
    logMatch(ctx, "statement : var_declaration", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitStmtExpr(CSubset_2205091Parser::StmtExprContext *ctx)
{
    string text = any_cast<string>(visit(ctx->expression_statement()));
    logMatch(ctx, "statement : expression_statement", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitStmtCompound(CSubset_2205091Parser::StmtCompoundContext *ctx)
{
    symbolTable->enterScope();
    string text = any_cast<string>(visit(ctx->compound_statement()));
    symbolTable->exitScope();

    logMatch(ctx, "statement : compound_statement", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitStmtFor(CSubset_2205091Parser::StmtForContext *ctx)
{
    string initStr = any_cast<string>(visit(ctx->init));
    string condStr = any_cast<string>(visit(ctx->cond));
    visit(ctx->expression());
    string exprStr = ctx->expression()->getText();
    string bodyStr = any_cast<string>(visit(ctx->body));

    string text = "for(" + initStr + condStr + exprStr + ")" + bodyStr;
    logMatch(ctx, "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitStmtIf(CSubset_2205091Parser::StmtIfContext *ctx)
{
    visit(ctx->expression());
    string exprStr = ctx->expression()->getText();
    string stmtStr = any_cast<string>(visit(ctx->statement()));

    string text = "if (" + exprStr + ")" + stmtStr;
    logMatch(ctx, "statement : IF LPAREN expression RPAREN statement", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitStmtIfElse(CSubset_2205091Parser::StmtIfElseContext *ctx)
{
    visit(ctx->expression());
    string exprStr = ctx->expression()->getText();
    string thenStr = any_cast<string>(visit(ctx->thenStmt));
    string elseStr = any_cast<string>(visit(ctx->elseStmt));

    string text = "if (" + exprStr + ")" + thenStr + "\nelse\n" + elseStr;
    logMatch(ctx, "statement : IF LPAREN expression RPAREN statement ELSE statement", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitStmtWhile(CSubset_2205091Parser::StmtWhileContext *ctx)
{
    visit(ctx->expression());
    string exprStr = ctx->expression()->getText();
    string stmtStr = any_cast<string>(visit(ctx->statement()));

    string text = "while (" + exprStr + ")" + stmtStr;
    logMatch(ctx, "statement : WHILE LPAREN expression RPAREN statement", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitStmtPrintln(CSubset_2205091Parser::StmtPrintlnContext *ctx)
{
    string name = ctx->ID()->getText();
    SymbolInfo *symbol = symbolTable->lookUpSymbolInfo(name);
    if (symbol == nullptr)
    {
        reportError(ctx, "Undeclared variable " + name);
    }

    string text = "printf(" + name + ");";
    logMatch(ctx, "statement : PRINTLN LPAREN ID RPAREN SEMICOLON", text);
    cout << "\n";
    return text;
}

any MyVisitor::visitStmtReturn(CSubset_2205091Parser::StmtReturnContext *ctx)
{
    visit(ctx->expression());
    string exprText = ctx->expression()->getText();

    string text = "return " + exprText + ";";
    logMatch(ctx, "statement : RETURN expression SEMICOLON", text);
    cout << "\n";
    return text;
}

// EXPRESSION STATEMENTS

any MyVisitor::visitExprStmtEmpty(CSubset_2205091Parser::ExprStmtEmptyContext *ctx)
{
    string text = ";";
    logMatch(ctx, "expression_statement : SEMICOLON", text);
    return text;
}

any MyVisitor::visitExprStmtExpr(CSubset_2205091Parser::ExprStmtExprContext *ctx)
{
    visit(ctx->expression());
    string exprText = ctx->expression()->getText();
    string text = exprText + ";";

    logMatch(ctx, "expression_statement : expression SEMICOLON", text);
    return text;
}

any MyVisitor::visitExprStmtNoSemiError(CSubset_2205091Parser::ExprStmtNoSemiErrorContext *ctx)
{
    visit(ctx->expression());
    string exprText = cleanExprText(ctx->expression()->getText());

    reportError(ctx, "syntax error, missing ';' after expression '" + exprText + "'");
    logMatch(ctx, "expression_statement : expression (missing SEMICOLON)", exprText);

    return exprText;
}

// VARIABLES & ACCESS

any MyVisitor::visitVarSimple(CSubset_2205091Parser::VarSimpleContext *ctx)
{
    string name = ctx->ID()->getText();
    SymbolInfo *symbol = symbolTable->lookUpSymbolInfo(name);
    if (symbol == nullptr)
    {
        reportError(ctx, "Undeclared variable " + name);
        logMatch(ctx, "variable : ID");
        return string("ERROR");
    }

    if (symbol->getIsArray())
    {
        reportError(ctx, "Type mismatch, " + name + " is an array");
        logMatch(ctx, "variable : ID");
        return string("ERROR");
    }

    logMatch(ctx, "variable : ID");
    return symbol->getDataType();
}

any MyVisitor::visitVarArray(CSubset_2205091Parser::VarArrayContext *ctx)
{
    string name = ctx->ID()->getText();
    SymbolInfo *symbol = symbolTable->lookUpSymbolInfo(name);

    string exprType = any_cast<string>(visit(ctx->expression()));

    if (symbol == nullptr)
    {
        reportError(ctx, "Undeclared variable " + name);
        logMatch(ctx, "variable : ID LTHIRD expression RTHIRD");
        return string("ERROR");
    }

    if (!symbol->getIsArray())
    {
        reportError(ctx, name + " not an array");
    }

    if (exprType == "VOID")
    {
        reportError(ctx, "Void function used in expression");
    }
    else if (exprType != "INT" && exprType != "ERROR")
    {
        reportError(ctx, "Expression inside third brackets not an integer");
    }

    logMatch(ctx, "variable : ID LTHIRD expression RTHIRD");
    return symbol->getDataType();
}

// EXPRESSIONS & LOGIC

any MyVisitor::visitExprLogic(CSubset_2205091Parser::ExprLogicContext *ctx)
{
    string t = any_cast<string>(visit(ctx->logic_expression()));
    logMatch(ctx, "expression : logic expression", cleanExprText(ctx->getText()));
    return t;
}

any MyVisitor::visitExprAssign(CSubset_2205091Parser::ExprAssignContext *ctx)
{
    string lhsType = any_cast<string>(visit(ctx->variable()));
    string rhsType = any_cast<string>(visit(ctx->logic_expression()));

    if (rhsType == "VOID")
    {
        reportError(ctx, "Void function used in expression");
    }

    if (lhsType != "ERROR" && rhsType != "ERROR" && rhsType != "VOID")
    {
        if (lhsType == "INT" && rhsType == "FLOAT")
        {
            reportError(ctx, "Type Mismatch");
        }
    }
    logMatch(ctx, "expression : variable ASSIGNOP logic_expression", cleanExprText(ctx->getText()));
    return lhsType;
}

any MyVisitor::visitLogicExprRel(CSubset_2205091Parser::LogicExprRelContext *ctx)
{
    string t = any_cast<string>(visit(ctx->rel_expression()));
    logMatch(ctx, "logic_expression : rel_expression", cleanExprText(ctx->getText()));
    return t;
}

any MyVisitor::visitLogicExprBinary(CSubset_2205091Parser::LogicExprBinaryContext *ctx)
{
    string leftType = any_cast<string>(visit(ctx->left));
    string rightType = any_cast<string>(visit(ctx->right));
    logMatch(ctx, "logic_expression : rel_expression LOGICOP rel_expression", cleanExprText(ctx->getText()));
    if (leftType == "ERROR" || rightType == "ERROR")
        return string("ERROR");
    return string("INT");
}

any MyVisitor::visitRelExprSimple(CSubset_2205091Parser::RelExprSimpleContext *ctx)
{
    string t = any_cast<string>(visit(ctx->simple_expression()));
    logMatch(ctx, "rel_expression : simple_expression", cleanExprText(ctx->getText()));
    return t;
}

any MyVisitor::visitRelExprBinary(CSubset_2205091Parser::RelExprBinaryContext *ctx)
{
    string leftType = any_cast<string>(visit(ctx->left));
    string rightType = any_cast<string>(visit(ctx->right));
    logMatch(ctx, "rel_expression : simple_expression RELOP simple_expression", cleanExprText(ctx->getText()));
    if (leftType == "ERROR" || rightType == "ERROR")
        return string("ERROR");
    return string("INT");
}

// SIMPLE EXPRESSIONS & TERMS

any MyVisitor::visitSimpleExprAdd(CSubset_2205091Parser::SimpleExprAddContext *ctx)
{
    string left = any_cast<string>(visit(ctx->simple_expression()));
    string right = any_cast<string>(visit(ctx->term()));

    if (left == "VOID" || right == "VOID")
    {
        reportError(ctx, "Void function used in expression");
    }

    logMatch(ctx, "simple_expression : simple_expression ADDOP term");
    if (left == "ERROR" || right == "ERROR")
        return string("ERROR");
    if (left == "FLOAT" || right == "FLOAT")
        return string("FLOAT");
    return string("INT");
}

any MyVisitor::visitSimpleExprTerm(CSubset_2205091Parser::SimpleExprTermContext *ctx)
{
    string t = any_cast<string>(visit(ctx->term()));
    logMatch(ctx, "simple_expression : term");
    return t;
}

any MyVisitor::visitSimpleExprAddAssignError(CSubset_2205091Parser::SimpleExprAddAssignErrorContext *ctx)
{
    // Visit simple_expression first to preserve log order
    auto res = visit(ctx->simple_expression());
    
    string op = ctx->ADDOP()->getText();
    reportError(ctx, "syntax error, invalid operand '=' after '" + op + "'");
    
    return res;
}

any MyVisitor::visitTermUnary(CSubset_2205091Parser::TermUnaryContext *ctx)
{
    string t = any_cast<string>(visit(ctx->unary_expression()));
    logMatch(ctx, "term : unary_expression");
    return t;
}

any MyVisitor::visitTermMul(CSubset_2205091Parser::TermMulContext *ctx)
{
    string left = any_cast<string>(visit(ctx->term()));
    string right = any_cast<string>(visit(ctx->unary_expression()));
    string op = ctx->MULOP()->getText();

    if (left == "VOID" || right == "VOID")
    {
        reportError(ctx, "Void function used in expression");
    }

    if (op == "%")
    {
        if ((left != "INT" && left != "ERROR") || (right != "INT" && right != "ERROR"))
        {
            reportError(ctx, "Non-Integer operand on modulus operator");
        }
        if (ctx->unary_expression()->getText() == "0")
        {
            reportError(ctx, "Modulus by Zero");
        }
        logMatch(ctx, "term : term MULOP unary_expression");
        return string("INT");
    }

    logMatch(ctx, "term : term MULOP unary_expression");
    if (left == "ERROR" || right == "ERROR")
        return string("ERROR");
    if (left == "FLOAT" || right == "FLOAT")
        return string("FLOAT");
    return string("INT");
}

any MyVisitor::visitUnaryAdd(CSubset_2205091Parser::UnaryAddContext *ctx)
{
    string t = any_cast<string>(visit(ctx->unary_expression()));
    logMatch(ctx, "unary_expression : ADDOP unary_expression");
    return t;
}

any MyVisitor::visitUnaryNot(CSubset_2205091Parser::UnaryNotContext *ctx)
{
    string t = any_cast<string>(visit(ctx->unary_expression()));
    logMatch(ctx, "unary_expression : NOT unary expression");
    return t;
}

any MyVisitor::visitUnaryFactor(CSubset_2205091Parser::UnaryFactorContext *ctx)
{
    string t = any_cast<string>(visit(ctx->factor()));
    logMatch(ctx, "unary_expression : factor");
    return t;
}

// FACTORS

any MyVisitor::visitFactorVar(CSubset_2205091Parser::FactorVarContext *ctx)
{
    string t = any_cast<string>(visit(ctx->variable()));
    logMatch(ctx, "factor : variable");
    return t;
}

any MyVisitor::visitFactorFuncCall(CSubset_2205091Parser::FactorFuncCallContext *ctx)
{
    string name = ctx->ID()->getText();
    auto argTypes = any_cast<vector<string>>(visit(ctx->argument_list()));

    SymbolInfo *symbol = symbolTable->lookUpSymbolInfo(name);
    if (symbol == nullptr)
    {
        reportError(ctx, "Undeclared function " + name);
        logMatch(ctx, "factor : ID LPAREN argument_list RPAREN");
        return string("ERROR");
    }

    auto &params = symbol->getParamList();
    if (params.size() != argTypes.size())
    {
        reportError(ctx, "Total number of arguments mismatch in function " + name);
    }
    else
    {
        for (size_t i = 0; i < params.size(); i++)
        {
            if (argTypes[i] == "ERROR")
                continue;
            if (argTypes[i] != params[i].type)
            {
                reportError(ctx, to_string(i + 1) + "th argument mismatch in function " + name);
                break;
            }
        }
    }

    logMatch(ctx, "factor : ID LPAREN argument_list RPAREN");
    return symbol->getDataType();
}

any MyVisitor::visitFactorParen(CSubset_2205091Parser::FactorParenContext *ctx)
{
    string t = any_cast<string>(visit(ctx->expression()));
    logMatch(ctx, "factor : LPAREN expression RPAREN");
    return t;
}

any MyVisitor::visitFactorConstInt(CSubset_2205091Parser::FactorConstIntContext *ctx)
{
    logMatch(ctx, "factor : CONST_INT");
    return string("INT");
}

any MyVisitor::visitFactorConstFloat(CSubset_2205091Parser::FactorConstFloatContext *ctx)
{
    string floatStr = ctx->CONST_FLOAT()->getText();
    logMatch(ctx, "factor : CONST_FLOAT", floatStr);
    return string("FLOAT");
}

any MyVisitor::visitFactorInc(CSubset_2205091Parser::FactorIncContext *ctx)
{
    string t = any_cast<string>(visit(ctx->variable()));
    logMatch(ctx, "factor : variable INCOP");
    return t;
}

any MyVisitor::visitFactorDec(CSubset_2205091Parser::FactorDecContext *ctx)
{
    string t = any_cast<string>(visit(ctx->variable()));
    logMatch(ctx, "factor : variable DECOP");
    return t;
}

// ARGUMENTS

any MyVisitor::visitArgListExpr(CSubset_2205091Parser::ArgListExprContext *ctx)
{
    auto v = any_cast<vector<string>>(visit(ctx->arguments()));
    logMatch(ctx, "argument_list : arguments");
    return v;
}

any MyVisitor::visitArgListEmpty(CSubset_2205091Parser::ArgListEmptyContext *ctx)
{
    return vector<string>{};
}

any MyVisitor::visitArgsMulti(CSubset_2205091Parser::ArgsMultiContext *ctx)
{
    auto v = any_cast<vector<string>>(visit(ctx->arguments()));
    string t = any_cast<string>(visit(ctx->logic_expression()));
    v.push_back(t);
    logMatch(ctx, "arguments : arguments COMMA logic_expression");
    return v;
}

any MyVisitor::visitArgsSingle(CSubset_2205091Parser::ArgsSingleContext *ctx)
{
    string t = any_cast<string>(visit(ctx->logic_expression()));
    vector<string> v{t};
    logMatch(ctx, "arguments : logic_expression");
    return v;
}