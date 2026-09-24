#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "antlr4-runtime.h"
#include "2205091_symbol_table.hpp"
#include "CSubset_2205091BaseVisitor.h"

using namespace std;

class MyVisitor : public CSubset_2205091BaseVisitor
{
private:
    SymbolTable *symbolTable;
    int errorCount;
    string currentType;

    void logMatch(antlr4::ParserRuleContext *ctx, const string &production);
    void logMatch(antlr4::ParserRuleContext *ctx, const std::string &production, int line);
    void logMatch(antlr4::ParserRuleContext *ctx, const std::string &production, const std::string &text, int line);
    void logMatch(antlr4::ParserRuleContext *ctx, const std::string &production, const std::string &text);

    void reportError(antlr4::ParserRuleContext *ctx, const string &message);
    void reportError(int line, const string &message);

public:
    MyVisitor(int bucketSize);

    virtual std::any visitStartRule(CSubset_2205091Parser::StartRuleContext *ctx) override;
    virtual std::any visitProgramSingleUnit(CSubset_2205091Parser::ProgramSingleUnitContext *ctx) override;
    virtual std::any visitProgramUnits(CSubset_2205091Parser::ProgramUnitsContext *ctx) override;
    virtual std::any visitUnitVarDecl(CSubset_2205091Parser::UnitVarDeclContext *ctx) override;
    virtual std::any visitUnitFuncDecl(CSubset_2205091Parser::UnitFuncDeclContext *ctx) override;
    virtual std::any visitUnitFuncDef(CSubset_2205091Parser::UnitFuncDefContext *ctx) override;

    virtual std::any visitFuncDeclWithParams(CSubset_2205091Parser::FuncDeclWithParamsContext *ctx) override;
    virtual std::any visitFuncDeclNoParams(CSubset_2205091Parser::FuncDeclNoParamsContext *ctx) override;
    virtual std::any visitFuncDefWithParams(CSubset_2205091Parser::FuncDefWithParamsContext *ctx) override;
    virtual std::any visitFuncDefNoParams(CSubset_2205091Parser::FuncDefNoParamsContext *ctx) override;

    virtual std::any visitParamListMulti(CSubset_2205091Parser::ParamListMultiContext *ctx) override;
    virtual std::any visitParamListMultiUnnamed(CSubset_2205091Parser::ParamListMultiUnnamedContext *ctx) override;
    virtual std::any visitParamSingleUnnamed(CSubset_2205091Parser::ParamSingleUnnamedContext *ctx) override;
    virtual std::any visitParamSingle(CSubset_2205091Parser::ParamSingleContext *ctx) override;
    virtual std::any visitParamADDOPError(CSubset_2205091Parser::ParamADDOPErrorContext *ctx) override;

    virtual std::any visitCompoundStmtWithStmts(CSubset_2205091Parser::CompoundStmtWithStmtsContext *ctx) override;
    virtual std::any visitCompoundStmtEmpty(CSubset_2205091Parser::CompoundStmtEmptyContext *ctx) override;

    virtual std::any visitVarDeclaration(CSubset_2205091Parser::VarDeclarationContext *ctx) override;

    virtual std::any visitTypeInt(CSubset_2205091Parser::TypeIntContext *ctx) override;
    virtual std::any visitTypeFloat(CSubset_2205091Parser::TypeFloatContext *ctx) override;
    virtual std::any visitTypeVoid(CSubset_2205091Parser::TypeVoidContext *ctx) override;

    virtual std::any visitDeclListArray(CSubset_2205091Parser::DeclListArrayContext *ctx) override;
    virtual std::any visitDeclVar(CSubset_2205091Parser::DeclVarContext *ctx) override;
    virtual std::any visitDeclArray(CSubset_2205091Parser::DeclArrayContext *ctx) override;
    virtual std::any visitDeclListVar(CSubset_2205091Parser::DeclListVarContext *ctx) override;
    virtual std::any visitDeclADDOPError(CSubset_2205091Parser::DeclADDOPErrorContext *ctx) override;

    virtual std::any visitSingleStmt(CSubset_2205091Parser::SingleStmtContext *ctx) override;
    virtual std::any visitMultipleStmts(CSubset_2205091Parser::MultipleStmtsContext *ctx) override;
    virtual std::any visitStmtVarDecl(CSubset_2205091Parser::StmtVarDeclContext *ctx) override;
    virtual std::any visitStmtExpr(CSubset_2205091Parser::StmtExprContext *ctx) override;
    virtual std::any visitStmtCompound(CSubset_2205091Parser::StmtCompoundContext *ctx) override;
    virtual std::any visitStmtFor(CSubset_2205091Parser::StmtForContext *ctx) override;
    virtual std::any visitStmtIf(CSubset_2205091Parser::StmtIfContext *ctx) override;
    virtual std::any visitStmtIfElse(CSubset_2205091Parser::StmtIfElseContext *ctx) override;
    virtual std::any visitStmtWhile(CSubset_2205091Parser::StmtWhileContext *ctx) override;
    virtual std::any visitStmtPrintln(CSubset_2205091Parser::StmtPrintlnContext *ctx) override;
    virtual std::any visitStmtReturn(CSubset_2205091Parser::StmtReturnContext *ctx) override;

    virtual std::any visitExprStmtEmpty(CSubset_2205091Parser::ExprStmtEmptyContext *ctx) override;
    virtual std::any visitExprStmtExpr(CSubset_2205091Parser::ExprStmtExprContext *ctx) override;
    virtual std::any visitExprStmtNoSemiError(CSubset_2205091Parser::ExprStmtNoSemiErrorContext *ctx) override;

    virtual std::any visitVarSimple(CSubset_2205091Parser::VarSimpleContext *ctx) override;
    virtual std::any visitVarArray(CSubset_2205091Parser::VarArrayContext *ctx) override;

    virtual std::any visitExprLogic(CSubset_2205091Parser::ExprLogicContext *ctx) override;
    virtual std::any visitExprAssign(CSubset_2205091Parser::ExprAssignContext *ctx) override;

    virtual std::any visitLogicExprRel(CSubset_2205091Parser::LogicExprRelContext *ctx) override;
    virtual std::any visitLogicExprBinary(CSubset_2205091Parser::LogicExprBinaryContext *ctx) override;

    virtual std::any visitRelExprSimple(CSubset_2205091Parser::RelExprSimpleContext *ctx) override;
    virtual std::any visitRelExprBinary(CSubset_2205091Parser::RelExprBinaryContext *ctx) override;

    virtual std::any visitSimpleExprAdd(CSubset_2205091Parser::SimpleExprAddContext *ctx) override;
    virtual std::any visitSimpleExprTerm(CSubset_2205091Parser::SimpleExprTermContext *ctx) override;
    virtual std::any visitSimpleExprAddAssignError(CSubset_2205091Parser::SimpleExprAddAssignErrorContext *ctx) override;

    virtual std::any visitTermUnary(CSubset_2205091Parser::TermUnaryContext *ctx) override;
    virtual std::any visitTermMul(CSubset_2205091Parser::TermMulContext *ctx) override;

    virtual std::any visitUnaryAdd(CSubset_2205091Parser::UnaryAddContext *ctx) override;
    virtual std::any visitUnaryNot(CSubset_2205091Parser::UnaryNotContext *ctx) override;
    virtual std::any visitUnaryFactor(CSubset_2205091Parser::UnaryFactorContext *ctx) override;

    virtual std::any visitFactorVar(CSubset_2205091Parser::FactorVarContext *ctx) override;
    virtual std::any visitFactorFuncCall(CSubset_2205091Parser::FactorFuncCallContext *ctx) override;
    virtual std::any visitFactorParen(CSubset_2205091Parser::FactorParenContext *ctx) override;
    virtual std::any visitFactorConstInt(CSubset_2205091Parser::FactorConstIntContext *ctx) override;
    virtual std::any visitFactorConstFloat(CSubset_2205091Parser::FactorConstFloatContext *ctx) override;
    virtual std::any visitFactorInc(CSubset_2205091Parser::FactorIncContext *ctx) override;
    virtual std::any visitFactorDec(CSubset_2205091Parser::FactorDecContext *ctx) override;

    virtual std::any visitArgListExpr(CSubset_2205091Parser::ArgListExprContext *ctx) override;
    virtual std::any visitArgListEmpty(CSubset_2205091Parser::ArgListEmptyContext *ctx) override;
    virtual std::any visitArgsMulti(CSubset_2205091Parser::ArgsMultiContext *ctx) override;
    virtual std::any visitArgsSingle(CSubset_2205091Parser::ArgsSingleContext *ctx) override;
};