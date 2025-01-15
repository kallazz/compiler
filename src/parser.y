%{
#include <iostream>
#include <string>

int yylex();
void yyerror(const std::string& message);
%}

%union { char *text; int number; }

%token <text> ID
%token <number> NUM
%token EOL ADD SUB MUL DIV MOD NEQ EQ GEQ GT LEQ LT ASSIGN COLON COMMA SEMI LPAREN RPAREN LBRACK RBRACK PROCEDURE
    PROGRAM IS BEGINN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR FROM TO DOWNTO ENDFOR READ WRITE INVALID

%left ADD SUB
%left MUL DIV

%%

input:
    /* empty */
    ;

%%

void yyerror(const std::string& message) {
    std::cerr << "Got error: " << message << '\n';
}

int main() {
    yyparse();

    return 0;
}
