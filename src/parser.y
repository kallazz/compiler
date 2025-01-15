%{
#include <iostream>
#include <string>

extern int yylineno;
int yylex();
void yyerror(const char *message);
%}

%locations

%union { char *text; int number; }

%token <text> NAME
%token <number> NUM
%token EOL ADD SUB MUL DIV MOD NEQ EQ GEQ GT LEQ LT ASSIGN COLON COMMA SEMICOLON LPAREN RPAREN LBRACK RBRACK PROCEDURE
    PROGRAM IS BEGINN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR FROM TO DOWNTO ENDFOR READ WRITE INVALID T

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
    PROGRAM IS declarations BEGINN commands END
    | PROGRAM IS BEGINN commands END
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
    NAME LPAREN arguments_declaration RPAREN
;

procedure_call:
    NAME LPAREN arguments RPAREN
;

declarations:
    declarations COMMA NAME
    | declarations COMMA NAME LBRACK NUM COLON NUM RBRACK
    | NAME
    | NAME LBRACK NUM COLON NUM RBRACK
;

arguments_declaration:
    arguments_declaration COMMA NAME
    | arguments_declaration COMMA T NAME
    | NAME
    | T NAME
;

arguments:
    arguments COMMA NAME
    | NAME
;

expression:
    value
    | value ADD value
    | value SUB value
    | value MUL value
    | value DIV value
    | value MOD value
;

condition:
    value EQ value
    | value NEQ value
    | value GT value
    | value LT value
    | value GEQ value
    | value LEQ value
;

value:
    NUM
    | identifier
;

identifier:
    NAME
    | NAME LBRACK NAME RBRACK
    | NAME LBRACK NUM RBRACK
;

%%

void yyerror(const char *message) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, message);
}
