%{
#include <stdio.h>
#include "./utils.h"
#include "y.tab.h"
%}

number  0|[1-9][0-9]*
id [~]*{letter}+{number}*{letter}*
letter  [a-zA-Z_]
%option noyywrap

%%
"Christmas"                                         {return CLASS;}
"private gifts"|"public gifts"			            {return ACCESS;}
"int"|"float"|"bells"|"tree"|"elf"                  {yylval.intVal = SetDataType(yytext); return VAR_TYPE;}
"It's beginning to look a lot like Christmas"       {return BGIN;}
"Christmas never ends"                              {return END;}
{number}                                            {yylval.intVal = atoi(yytext); return INTEGER;}
{number}"."{number}                                 {yylval.floatVal = atof(yytext); return FLOAT;}
["][^"]*["]                                         {yylval.strVal = strdup(yytext + 1); yylval.strVal[strlen(yylval.strVal)-1] = 0; return STRING;}
"while Christmas goes on"                           {return WHILE;}
"for each gift"                                     {return FOR;}
"if you love Christmas you get"                     {return IF;}
"else you're a Grinch"                              {return ELSE;}
"reindeer"                                          {return CONST;}
"Eval"                                              {return EVAL;}
{id}[ \t]*\[				                        {yylval.strVal = strdup(yytext); yyless(yyleng-1); return ID_VECTOR;}
{id}						                        {yylval.strVal = strdup(yytext); return ID;}
"-="		 				                        {yylval.strVal = strdup(yytext); return SUB_ASSIGN; }
"+="						                        {yylval.strVal = strdup(yytext); return ADD_ASSIGN; }
"*="		 				                        {yylval.strVal = strdup(yytext); return MUL_ASSIGN; }
"/="		 				                        {yylval.strVal = strdup(yytext); return DIV_ASSIGN; }
"%="						                        {yylval.strVal = strdup(yytext); return MOD_ASSIGN; }
"&&"|"||"|"<="|">="|"=="|"!="|"<"|">"               {return OP; }
"="			 			                            {return('='); }
"!"			 			                            {return('!'); }
"-"			 			                            {return('-'); }
"+"			 			                            {return('+'); }
"*"			 			                            {return('*'); }
"/"			 			                            {return('/'); }
"%"			 			                            {return('%'); }
[ \t]                                               ;
\n                                                  {yylineno++;}
.                                                   {return yytext[0];}
%%