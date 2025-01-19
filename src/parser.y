%code requires {
    #include "AbstractSyntaxTree.hpp"

    #include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/ExpressionNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/ValueNode.hpp"

    #include "AbstractSyntaxTreeStatementNodes/AbstractSyntaxTreeStatementNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/ArgumentsNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/ArgumentsDeclarationNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/AssignmentNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/CommandsNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/DeclarationsNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/ForLoopNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/IfNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/MainNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/ProcedureCallNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/ProcedureHeadNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/ProceduresNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/ReadNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/RepeatLoopNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/WhileLoopNode.hpp"
    #include "AbstractSyntaxTreeStatementNodes/WriteNode.hpp"

    #include "enums/ComparsionOperator.hpp"
    #include "enums/MathematicalOperator.hpp"

    #include <iostream>
    #include <memory>
    #include <string>
}

%code {
    extern int yylineno;
    int yylex();
    void yyerror(const AbstractSyntaxTree&, const std::string& message);
}

%locations

%parse-param { AbstractSyntaxTree& tree }

%union {
    char *text;
    long long number;

    ConditionNode* conditionNode;
    ExpressionNode* expressionNode;
    IdentifierNode* identifierNode;
    ValueNode* valueNode;

    AbstractSyntaxTreeStatementNode* astStatementNode;
    ArgumentsNode* argumentsNode;
    ArgumentsDeclarationNode* argumentsDeclarationNode;
    CommandsNode* commandsNode;
    DeclarationsNode* declarationsNode;
    ProcedureCallNode* procedureCallNode;
    ProcedureHeadNode* procedureHeadNode;
    ProceduresNode* proceduresNode;
    MainNode* mainNode;
}

%token <text> NAME
%token <number> NUM
%token EOL ADD SUB MUL DIV MOD NEQ EQ GEQ GT LEQ LT ASSIGN COLON COMMA SEMICOLON LPAREN RPAREN LBRACK RBRACK PROCEDURE
    PROGRAM IS BEGINN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR FROM TO DOWNTO ENDFOR READ WRITE INVALID T

%type <conditionNode> condition
%type <expressionNode> expression
%type <identifierNode> identifier
%type <valueNode> value

%type <astStatementNode> command
%type <argumentsNode> arguments
%type <argumentsDeclarationNode> arguments_declaration
%type <commandsNode> commands
%type <declarationsNode> declarations
%type <procedureCallNode> procedure_call
%type <procedureHeadNode> procedure_head
%type <proceduresNode> procedures
%type <mainNode> main

%destructor { delete $$; } <valueNode>
%destructor { delete $$; } <identifierNode>
%destructor { delete $$; } <expressionNode>
%destructor { delete $$; } <conditionNode>
%destructor { delete $$; } <argumentsNode>
%destructor { delete $$; } <astStatementNode>
%destructor { delete $$; } <argumentsDeclarationNode>
%destructor { delete $$; } <commandsNode>
%destructor { delete $$; } <declarationsNode>
%destructor { delete $$; } <procedureCallNode>
%destructor { delete $$; } <procedureHeadNode>
%destructor { delete $$; } <proceduresNode>
%destructor { delete $$; } <mainNode>
%destructor { delete $$; } NAME

%left EQ NEQ
%left GT GEQ LT LEQ
%left ADD SUB
%left MUL DIV

%start whole_program

%%

whole_program:
    procedures main {
        tree.setProceduresNode($1);
        tree.setMainNode($2);
    }
;

procedures:
    procedures PROCEDURE procedure_head IS declarations BEGINN commands END {
        $$ = $1;
        $$->addProcedure($3, $5, $7);
    }
    | procedures PROCEDURE procedure_head IS BEGINN commands END {
        $$ = $1;
        $$->addProcedure($3, $6);
    }
    | /* epsilon */ {
        $$ = new ProceduresNode(@$.first_line);
    }
;

main:
    PROGRAM IS declarations BEGINN commands END { $$ = new MainNode(@1.first_line, $3, $5); }
    | PROGRAM IS BEGINN commands END { $$ = new MainNode(@1.first_line, $4); }
;

commands:
    commands command {
        $$ = $1;
        $$->addCommand($2);
    }
    | command {
        $$ = new CommandsNode(@1.first_line);
        $$->addCommand($1);
    }
;

command:
    identifier ASSIGN expression SEMICOLON { $$ = new AssignmentNode(@1.first_line, $1, $3); }
    | IF condition THEN commands ELSE commands ENDIF { $$ = new IfNode(@1.first_line, $2, $4, $6); }
    | IF condition THEN commands ENDIF { $$ = new IfNode(@1.first_line, $2, $4); }
    | WHILE condition DO commands ENDWHILE { $$ = new WhileLoopNode(@1.first_line, $2, $4); }
    | REPEAT commands UNTIL condition SEMICOLON { $$ = new RepeatLoopNode(@1.first_line, $2, $4); }
    | FOR NAME FROM value TO value DO commands ENDFOR { $$ = new ForLoopNode(@1.first_line, $2, $4, $6, $8, true); free($2); }
    | FOR NAME FROM value DOWNTO value DO commands ENDFOR { $$ = new ForLoopNode(@1.first_line, $2, $4, $6, $8, false); free($2); }
    | procedure_call SEMICOLON { $$ = $1; }
    | READ identifier SEMICOLON { $$ = new ReadNode(@1.first_line, $2); }
    | WRITE value SEMICOLON { $$ = new WriteNode(@1.first_line, $2); }
;

procedure_head:
    NAME LPAREN arguments_declaration RPAREN { $$ = new ProcedureHeadNode(@1.first_line, $1, $3); free($1); }
;

procedure_call:
    NAME LPAREN arguments RPAREN { $$ = new ProcedureCallNode(@1.first_line, $1, $3); free($1); }
;

declarations:
    declarations COMMA NAME {
        $1->addDeclaration(@3.first_line, $3);
        $$ = $1;
        free($3);
    }
    | declarations COMMA NAME LBRACK NUM COLON NUM RBRACK {
        $1->addDeclaration(@3.first_line, $3, $5, $7);
        $$ = $1;
        free($3);
    }
    | NAME {
        $$ = new DeclarationsNode(@1.first_line);
        $$->addDeclaration(@1.first_line, $1);
        free($1);
    }
    | NAME LBRACK NUM COLON NUM RBRACK {
        $$ = new DeclarationsNode(@1.first_line);
        $$->addDeclaration(@1.first_line, $1, $3, $5);
        free($1);
    }
;

arguments_declaration:
    arguments_declaration COMMA NAME {
        $$ = $1;
        $$->addArgumentDeclaration(@3.first_line, $3, false);
        free($3);
    }
    | arguments_declaration COMMA T NAME {
        $$ = $1;
        $$->addArgumentDeclaration(@3.first_line, $4, true);
        free($4);
    }
    | NAME {
        $$ = new ArgumentsDeclarationNode(@1.first_line);
        $$->addArgumentDeclaration(@1.first_line, $1, false);
        free($1);
    }
    | T NAME {
        $$ = new ArgumentsDeclarationNode(@1.first_line);
        $$->addArgumentDeclaration(@1.first_line, $2, true);
        free($2);
    }
;

arguments:
    arguments COMMA NAME {
        $1->addArgument(@3.first_line, $3);
        $$ = $1;
        free($3);
    }
    | NAME {
        $$ = new ArgumentsNode(@1.first_line);
        $$->addArgument(@1.first_line, $1);
        free($1);
    }
;

expression:
    value { $$ = new ExpressionNode(@1.first_line, $1); }
    | value ADD value { $$ = new ExpressionNode(@1.first_line, $1, $3, MathematicalOperator::ADD); }
    | value SUB value { $$ = new ExpressionNode(@1.first_line, $1, $3, MathematicalOperator::SUBTRACT); }
    | value MUL value { $$ = new ExpressionNode(@1.first_line, $1, $3, MathematicalOperator::MULTIPLY); }
    | value DIV value { $$ = new ExpressionNode(@1.first_line, $1, $3, MathematicalOperator::DIVIDE); }
    | value MOD value { $$ = new ExpressionNode(@1.first_line, $1, $3, MathematicalOperator::MODULO); }
;

condition:
    value EQ value { $$ = new ConditionNode(@1.first_line, $1, $3, ComparsionOperator::EQUAL); }
    | value NEQ value { $$ = new ConditionNode(@1.first_line, $1, $3, ComparsionOperator::NOT_EQUAL); }
    | value GT value { $$ = new ConditionNode(@1.first_line, $1, $3, ComparsionOperator::GREATER_THAN); }
    | value LT value { $$ = new ConditionNode(@1.first_line, $1, $3, ComparsionOperator::LESS_THAN); }
    | value GEQ value { $$ = new ConditionNode(@1.first_line, $1, $3, ComparsionOperator::GREATER_THAN_OR_EQUAL); }
    | value LEQ value { $$ = new ConditionNode(@1.first_line, $1, $3, ComparsionOperator::LESS_THAN_OR_EQUAL); }
;

value:
    NUM { $$ = new ValueNode(@1.first_line, $1); }
    | identifier { $$ = new ValueNode(@1.first_line, $1); }
;

identifier:
    NAME { $$ = new IdentifierNode(@1.first_line, $1); free($1); }
    | NAME LBRACK NAME RBRACK { $$ = new IdentifierNode(@1.first_line, $1, $3); free($1); free($3); }
    | NAME LBRACK NUM RBRACK { $$ = new IdentifierNode(@1.first_line, $1, $3); free($1); }

%%

void yyerror(const AbstractSyntaxTree&, const std::string& message) {
    std::cerr << "Error at line " << yylineno << ": " << message << '\n';
}
