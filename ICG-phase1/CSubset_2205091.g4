grammar CSubset_2205091;
import Lexer_2205091;

start : program # StartRule ;

program
    : program unit # ProgramUnits
    | unit         # ProgramSingleUnit
    ;

unit
    : var_declaration  # UnitVarDecl
    | func_declaration # UnitFuncDecl
    | func_definition  # UnitFuncDef
    ;

func_declaration
    : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON # FuncDeclWithParams
    | type_specifier ID LPAREN RPAREN SEMICOLON                # FuncDeclNoParams
    ;

func_definition
    : type_specifier ID LPAREN parameter_list RPAREN compound_statement # FuncDefWithParams
    | type_specifier ID LPAREN RPAREN compound_statement                # FuncDefNoParams
    ;

parameter_list
    : parameter_list COMMA type_specifier ID # ParamListMulti
    | parameter_list COMMA type_specifier    # ParamListMultiUnnamed
    | type_specifier ID                      # ParamSingle
    | type_specifier                         # ParamSingleUnnamed
    | type_specifier ADDOP                   # ParamADDOPError
    ;

compound_statement
    : LCURL statements RCURL # CompoundStmtWithStmts
    | LCURL RCURL            # CompoundStmtEmpty
    ;

var_declaration
    : type_specifier declaration_list SEMICOLON # VarDeclaration
    ;

type_specifier
    : INT   # TypeInt
    | FLOAT # TypeFloat
    | VOID  # TypeVoid
    ;

declaration_list
    : declaration_list COMMA ID                         # DeclListVar
    | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD # DeclListArray
    | ID                                                # DeclVar
    | ID LTHIRD CONST_INT RTHIRD                        # DeclArray
    | declaration_list ADDOP ID                         # DeclADDOPError
    ;

statements
    : statement            # SingleStmt
    | statements statement # MultipleStmts
    ;

statement
    : var_declaration                                                                                        # StmtVarDecl
    | expression_statement                                                                                   # StmtExpr
    | compound_statement                                                                                     # StmtCompound
    | FOR LPAREN init=expression_statement cond=expression_statement incr=expression RPAREN body=statement # StmtFor
    | IF LPAREN expression RPAREN statement                                                                  # StmtIf
    | IF LPAREN expression RPAREN thenStmt=statement ELSE elseStmt=statement                                 # StmtIfElse
    | WHILE LPAREN expression RPAREN statement                                                               # StmtWhile
    | PRINTLN LPAREN ID RPAREN SEMICOLON                                                                     # StmtPrintln
    | RETURN expression SEMICOLON                                                                            # StmtReturn
    ;

expression_statement
    : SEMICOLON            # ExprStmtEmpty
    | expression SEMICOLON # ExprStmtExpr
    | expression           # ExprStmtNoSemiError
    ;

variable
    : ID                          # VarSimple
    | ID LTHIRD expression RTHIRD # VarArray
    ;

expression
    : logic_expression                    # ExprLogic
    | variable ASSIGNOP logic_expression # ExprAssign
    ;

logic_expression
    : rel_expression                                   # LogicExprRel
    | left=rel_expression LOGICOP right=rel_expression # LogicExprBinary
    ;

rel_expression
    : simple_expression                             # RelExprSimple
    | left=simple_expression RELOP right=simple_expression # RelExprBinary
    ;

simple_expression
    : term                                  # SimpleExprTerm
    | simple_expression ADDOP term          # SimpleExprAdd
    | simple_expression ADDOP ASSIGNOP      # SimpleExprAddAssignError
    ;

term
    : unary_expression            # TermUnary
    | term MULOP unary_expression # TermMul
    ;

unary_expression
    : ADDOP unary_expression # UnaryAdd
    | NOT unary_expression   # UnaryNot
    | factor                 # UnaryFactor
    ;

factor
    : variable                       # FactorVar
    | ID LPAREN argument_list RPAREN # FactorFuncCall
    | LPAREN expression RPAREN       # FactorParen
    | CONST_INT                      # FactorConstInt
    | CONST_FLOAT                    # FactorConstFloat
    | variable INCOP                 # FactorInc
    | variable DECOP                 # FactorDec
    ;

argument_list
    : arguments # ArgListExpr
    |           # ArgListEmpty
    ;

arguments
    : arguments COMMA logic_expression # ArgsMulti
    | logic_expression                 # ArgsSingle
    ;