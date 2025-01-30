%option noyywrap nounput yylineno

%{
    #include "parser.tab.h"

    #include <stdlib.h>

    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
%}

%%

[[:blank:]]+    { /* Ignore whitespace */ }
#.*             { /* Ignore comments */ }
\n              { /* Ignore endlines */ }
[_a-z]+         { yylval.text = strdup(yytext); return NAME; }
[1-9][0-9]*|0   { yylval.number = atoll(yytext); return NUM; }
"+"             { return ADD; }
"-"             { return SUB; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"             { return MOD; }
"!="            { return NEQ; }
"="             { return EQ; }
">="            { return GEQ; }
">"             { return GT; }
"<="            { return LEQ; }
"<"             { return LT; }
":="            { return ASSIGN; }
":"             { return COLON; }
","             { return COMMA; }
";"             { return SEMICOLON; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRACK; }
"]"             { return RBRACK; }
"PROCEDURE"     { return PROCEDURE; }
"PROGRAM"       { return PROGRAM; }
"IS"            { return IS; }
"BEGIN"         { return BEGINN; }
"END"           { return END; }
"IF"            { return IF; }
"THEN"          { return THEN; }
"ELSE"          { return ELSE; }
"ENDIF"         { return ENDIF; }
"WHILE"         { return WHILE; }
"DO"            { return DO; }
"ENDWHILE"      { return ENDWHILE; }
"REPEAT"        { return REPEAT; }
"UNTIL"         { return UNTIL; }
"FOR"           { return FOR; }
"FROM"          { return FROM; }
"TO"            { return TO; }
"DOWNTO"        { return DOWNTO; }
"ENDFOR"        { return ENDFOR; }
"READ"          { return READ; }
"WRITE"         { return WRITE; }
"T"             { return T; }
.               { return INVALID; }

%%
