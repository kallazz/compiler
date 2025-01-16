%code requires {
    #include "AbstractSyntaxTree.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/ArgumentsNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/ArgumentsDeclarationNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/DeclarationsNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/ExpressionNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/ProcedureCallNode.hpp"
    #include "AbstractSyntaxTreeExpressionNodes/ValueNode.hpp"
    #include "enums/ComparsionOperator.hpp"
    #include "enums/MathematicalOperator.hpp"
    #include <iostream>
    #include <memory>
    #include <typeinfo>
}

%code {
    extern int yylineno;
    int yylex();
    void yyerror(const std::shared_ptr<AbstractSyntaxTree> tree, const char *message);
}

%locations

%parse-param { std::shared_ptr<AbstractSyntaxTree> tree }

%union {
    char *text;
    long long number;
    ArgumentsNode* argumentsNode;
    ArgumentsDeclarationNode* argumentsDeclarationNode;
    ConditionNode* conditionNode;
    DeclarationsNode* declarationsNode;
    ExpressionNode* expressionNode;
    IdentifierNode* identifierNode;
    ProcedureCallNode* procedureCallNode;
    ValueNode* valueNode;
}

%token <text> NAME
%token <number> NUM
%token EOL ADD SUB MUL DIV MOD NEQ EQ GEQ GT LEQ LT ASSIGN COLON COMMA SEMICOLON LPAREN RPAREN LBRACK RBRACK PROCEDURE
    PROGRAM IS BEGINN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR FROM TO DOWNTO ENDFOR READ WRITE INVALID T

%type <argumentsNode> arguments
%type <argumentsDeclarationNode> arguments_declaration
%type <conditionNode> condition
%type <declarationsNode> declarations
%type <expressionNode> expression
%type <identifierNode> identifier
%type <procedureCallNode> procedure_call
%type <valueNode> value

%left EQ NEQ
%left GT GEQ LT LEQ
%left ADD SUB
%left MUL DIV

%start whole_program

%%

whole_program:
    procedures main
;

procedures:
    PROCEDURE procedure_head IS declarations BEGINN commands END
    | PROCEDURE procedure_head IS BEGINN commands END
    | /* epsilon */
;

main:
    PROGRAM IS declarations BEGINN commands END {
    }
    | PROGRAM IS BEGINN commands END {
    }
;
;

commands:
    commands command
    | command
;

command:
    identifier ASSIGN expression SEMICOLON
    | IF condition THEN commands ELSE commands ENDIF
    | IF condition THEN commands ENDIF
    | WHILE condition DO commands ENDWHILE
    | REPEAT commands UNTIL condition SEMICOLON
    | FOR NAME FROM value TO value DO commands ENDFOR
    | FOR NAME FROM value DOWNTO value DO commands ENDFOR
    | procedure_call SEMICOLON
    | READ identifier SEMICOLON
    | WRITE value SEMICOLON
;

procedure_head:
    NAME LPAREN arguments_declaration RPAREN {
    }
;

procedure_call:
    NAME LPAREN arguments RPAREN { $$ = new ProcedureCallNode($1, $3); }
;

declarations:
    declarations COMMA NAME {
        $1->addDeclaration($3);
        $$ = $1;
    }
    | declarations COMMA NAME LBRACK NUM COLON NUM RBRACK {
        $1->addDeclaration($3, $5, $7);
        $$ = $1;
    }
    | NAME {
        $$ = new DeclarationsNode();
        $$->addDeclaration($1);
    }
    | NAME LBRACK NUM COLON NUM RBRACK {
        $$ = new DeclarationsNode();
        $$->addDeclaration($1, $3, $5);
    }
;

arguments_declaration:
    arguments_declaration COMMA NAME {
        $$ = $1;
        $$->addArgumentDeclaration($3, false);
    }
    | arguments_declaration COMMA T NAME {
        $$ = $1;
        $$->addArgumentDeclaration($4, true);
    }
    | NAME {
        $$ = new ArgumentsDeclarationNode();
        $$->addArgumentDeclaration($1, false);
    }
    | T NAME {
        $$ = new ArgumentsDeclarationNode();
        $$->addArgumentDeclaration($2, true);
    }
;

arguments:
    arguments COMMA NAME {
        $1->addArgument($3);
        $$ = $1;
    }
    | NAME {
        $$ = new ArgumentsNode();
        $$->addArgument($1);
    }
;

expression:
    value { $$ = new ExpressionNode($1); }
    | value ADD value { $$ = new ExpressionNode($1, $3, MathematicalOperator::ADD); }
    | value SUB value { $$ = new ExpressionNode($1, $3, MathematicalOperator::SUBTRACT); }
    | value MUL value { $$ = new ExpressionNode($1, $3, MathematicalOperator::MULTIPLY); }
    | value DIV value { $$ = new ExpressionNode($1, $3, MathematicalOperator::DIVIDE); }
    | value MOD value { $$ = new ExpressionNode($1, $3, MathematicalOperator::MODULO); }
;

condition:
    value EQ value { $$ = new ConditionNode($1, $3, ComparsionOperator::EQUAL); }
    | value NEQ value { $$ = new ConditionNode($1, $3, ComparsionOperator::NOT_EQUAL); }
    | value GT value { $$ = new ConditionNode($1, $3, ComparsionOperator::GREATER_THAN); }
    | value LT value { $$ = new ConditionNode($1, $3, ComparsionOperator::LESS_THAN); }
    | value GEQ value { $$ = new ConditionNode($1, $3, ComparsionOperator::GREATER_THAN_OR_EQUAL); }
    | value LEQ value { $$ = new ConditionNode($1, $3, ComparsionOperator::LESS_THAN_OR_EQUAL); }
;

value:
    NUM { $$ = new ValueNode($1); }
    | identifier { $$ = new ValueNode($1); }
;

identifier:
    NAME { $$ = new IdentifierNode($1); }
    | NAME LBRACK NAME RBRACK { $$ = new IdentifierNode($1, $3); }
    | NAME LBRACK NUM RBRACK { $$ = new IdentifierNode($1, $3); }

%%

void yyerror(const std::shared_ptr<AbstractSyntaxTree> tree, const char *message) {
    (void) tree; // to silence unused variable compiler warning
    fprintf(stderr, "Error at line %d: %s\n", yylineno, message);
}
