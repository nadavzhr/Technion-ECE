%{
    /* Declarations section */
    #include <stdio.h>
	#include <stdlib.h>
	#include <map>
	#include <iostream>
	#include <string>
    #include "part3_helpers.hpp"
    #include "part3.tab.hpp"
    using namespace std;
    extern "C" int yylex();
    void handle_error();
%}

/* Options */
%option yylineno
%option noyywrap

%option outfile="part3-lex.cpp"

/* Regular Expression Definitions */
digit       [0-9]
letter      [a-zA-Z]
whitespace  [ \t\n\r]

/* Complex Tokens*/
sign        \(|\)|\{|\}|,|;|:
ellipsis     \.\.\.
id          {letter}({letter}|{digit}|_)*
integernum  {digit}+
realnum     {digit}+\.{digit}+ 
string      \"(?:(\\["nt])|[^\\\"\n\r])*\"
relop       ==|<>|<=|>=|<|>
addop       \+|-
mulop       \*|\/
assign      =
and         &&
or          \|\|
not         !
comment     #[^\n\r]*

/* Rules Section in Descending Priority */
%%
int {
    return tk_int;
}
float {
    return tk_float;
}
void {
    return tk_void;
}
write {
    return tk_write;
}
read {
    return tk_read;
}
va_arg {
    return tk_va_arg;
}
while {
    return tk_while;
}
do {
    return tk_do;
}
if {
    return tk_if;
}
then {
    return tk_then;
}
else {
    return tk_else;
}
return {
    return tk_return;
}
{id}                            { yylval.name = strdup(yytext); 
                                  return tk_id; 
                                }
{integernum}                    { yylval.name = strdup(yytext);
                                  return tk_integernum; 
                                }
{realnum}                       { yylval.name = strdup(yytext); 
                                 return tk_realnum; 
                                }
{string}                        {
                                char* string = yytext;
                                string[yyleng-1] = 0; // Remove the first quote
                                string++; // Remove the last quote
                                yylval.name = strdup(string);
                                return tk_string;
                                }
{sign}                          {
                                yylval.name = strdup(yytext);
				                return yytext[0];
                                }
{ellipsis}                      { yylval.name = strdup(yytext); return tk_ellipsis; }
{relop}                         { yylval.name = strdup(yytext); return tk_relop; }
{addop}                         { yylval.name = strdup(yytext); return tk_addop; }
{mulop}                         { yylval.name = strdup(yytext); return tk_mulop; }
{assign}                        { yylval.name = strdup(yytext); return tk_assign; }
{and}                           { yylval.name = strdup(yytext); return tk_and; }
{or}                            { yylval.name = strdup(yytext); return tk_or; }
{not}                           { yylval.name = strdup(yytext); return tk_not; }
{whitespace}                    { /* Ignore whitespace */ }
{comment}                       { /* Ignore comments */ }
.                               { handle_error(); }

%%
// error handling function
void handle_error() {
    printf("Lexical error: '%s' in line number %d\n", yytext, yylineno);
    exit(1);
}