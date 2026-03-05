%option noyywrap

%{
#include <stdio.h>
#include <ctype.h>

extern int yylineno;
extern char *curfilename;

void addref(char *word, char *filename, int lineno);
%}

WORD   [A-Za-z]+

%%

{WORD}        { addref(yytext, curfilename, yylineno); }

\n            { yylineno++; }

[ \t]+        ;

.             ;

%%
