%{
    #include <stdio.h>
    #include <string.h>
    #include "./utils.h"

    int yylex();
    void yyerror(const char *);
    bool ProgramError();

    extern FILE* yyin;
    extern char* yytext;
    extern int yylineno;
%}

%union {
    int intVal;
    float floatVal;
    bool boolVal;
    char *strVal;
    struct variable var;
    struct Parameter parameter;
};

%token CLASS VAR_TYPE
%token BGIN END
%token INTEGER FLOAT STRING
%token WHILE FOR IF ELSE
%token ID ID_VECTOR
%token LEFT_OP RIGHT_OP DONE OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN
%token XOR_ASSIGN ACCESS CONST EVAL

%start progr

%type<intVal> INTEGER
%type<var> oper_variabila oper_variabila_eval
%type<intVal> VAR_TYPE apel_func
%type<strVal> ID ID_VECTOR STRING lista_param_apel_func lista_decl_param
%type<strVal> oper_asignare ADD_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN SUB_ASSIGN
%type<floatVal> FLOAT
%type<parameter> apel_func_param

%left '=' ADD_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN SUB_ASSIGN
%left '+' '-'
%left '*' '/' '%'
%left OP

%%
progr : declaratii instructiuni {printf("program corect sintactic\n");}
	;
		
declaratii : declaratie '?'
	     | declaratie '?' declaratii
	     ;

declaratie : declaratie_variabila_l
           | declaratie_functie '{' instructiuni_functie '}' {nrParams = 0;}
           | declaratie_functie '{''}' {nrParams = 0;}
           | declaratie_vector
           | declaratie_variabila_const
           | declaratie_clasa          
	   ;

declaratie_variabila_l: VAR_TYPE declaratie_variabila
                    ;

declaratie_variabila: declaratie_variabila ',' ID {AddNewVariable($<intVal>0, $3); if(ProgramError()) {return -1;}}
                    | ID {AddNewVariable($<intVal>0, $1); if(ProgramError()) {return -1;}}
                    | declaratie_variabila ',' ID '=' INTEGER {AddNewVariableInit($<intVal>0, $3, $5); if(ProgramError()) {return -1;}}
                    | declaratie_variabila ',' ID '=' FLOAT  {AddNewVariableInit($<intVal>0, $3, $5); if(ProgramError()) {return -1;}}
                    | declaratie_variabila ',' ID '=' STRING  {AddNewVariableInit($<intVal>0, $3, $5); if(ProgramError()) {return -1;}}
                    | ID '=' INTEGER {AddNewVariableInit($<intVal>0, $1, $3); if(ProgramError()) {return -1;}}
                    | ID '=' FLOAT  {AddNewVariableInit($<intVal>0, $1, $3); if(ProgramError()) {return -1;}}
                    | ID '=' STRING  {AddNewVariableInit($<intVal>0, $1, $3); if(ProgramError()) {return -1;}}
                    ;

declaratie_functie: VAR_TYPE ID '(' lista_decl_param ')' {AddNewFunctionWithParameters($1, $2); if(ProgramError()) {return -1;}}
                    | VAR_TYPE ID '(' ')' {AddNewFunction($1, $2); if(ProgramError()) {return -1;}}
                    ;

instructiuni_functie: instructiuni
                     | declaratii
                     ;

declaratie_vector: VAR_TYPE ID_VECTOR '[' INTEGER ']' {DeclareVector($1, $2, $4); if(ProgramError()) {return -1;}}
                  ;

declaratie_variabila_const: CONST VAR_TYPE ID '=' INTEGER {AddNewConstant($2, $3, $5); if(ProgramError()) {return -1;}}
                          | CONST VAR_TYPE ID '=' FLOAT  {AddNewConstant($2, $3, $5); if(ProgramError()) {return -1;}}
                          | CONST VAR_TYPE ID '=' STRING  {AddNewConstant($2, $3, $5); if(ProgramError()) {return -1;}}
                          ;

declaratie_clasa: CLASS ID '{' ACCESS ':' declaratii '}' {AddNewClass($2); if(ProgramError()) {return -1;}}
                 ;

lista_decl_param: VAR_TYPE {parameters[nrParams].dataType = $1; ++nrParams;}
                      | VAR_TYPE ',' lista_decl_param {parameters[nrParams].dataType = $1; ++nrParams;}
                      ;

instructiuni: BGIN instructiuni END
		;

instructiuni : enunt '?'
	     | instructiuni enunt '?'
             ;

enunt: atribuire
         | apel_func
         | EVAL '(' oper_variabila_eval ')'                   {Eval($3); if(ProgramError()) {return -1;}}
         | enunt_control
         | oper_variabila 
         ;

atribuire: ID '=' oper_variabila                        {AssignVarValue($1, $3); if(ProgramError()) {return -1;}}
          | ID oper_asignare oper_variabila                  {struct variable var1, rezVar; var1 = GetVariable($1); rezVar = $3; rezVar = OperatorFunction(var1, $2, rezVar); 
                                                AssignVarValue(var1.varName, rezVar);}
          | ID_VECTOR '[' INTEGER ']' '=' oper_variabila {AssignVectorValue($1, $3, $6); if(ProgramError()) {return -1;}}
          | ID_VECTOR '[' ID ']' '=' oper_variabila      {struct variable var = GetVariable($3); if(ProgramError()) {return -1;} if(var.dataType != INT_t || 
                                                var.isInitialized == false) {yyerror("Error : vector index can only be an initialized integer! Merry Christmas!"); 
                                                return -1;} AssignVectorValue($1, var.value.intVal, $6);}
          ;

oper_variabila_eval : oper_variabila '+' oper_variabila     {$$ = OperatorFunction($1, "+", $3); if(ProgramError()) {return -1;}}
     | oper_variabila '-' oper_variabila      {$$ = OperatorFunction($1, "-", $3); if(ProgramError()) {return -1;}}
     | oper_variabila '*' oper_variabila      {$$ = OperatorFunction($1, "*", $3); if(ProgramError()) {return -1;}}
     | oper_variabila '/' oper_variabila      {$$ = OperatorFunction($1, "/", $3); if(ProgramError()) {return -1;}} 
     | oper_variabila '%' oper_variabila      {$$ = OperatorFunction($1, "%", $3); if(ProgramError()) {return -1;}}
     | ID                   {yyerror("Error : Eval not used correctly! Merry Christmas!"); return -1;}
     | INTEGER                   {yyerror("Error : Eval not used correctly! Merry Christmas!"); return -1;}
     | FLOAT                   {yyerror("Error : Eval not used correctly! Merry Christmas!"); return -1;}
     | STRING                   {yyerror("Error : Eval not used correctly! Merry Christmas!"); return -1;}
     ;

oper_variabila : oper_variabila '+' oper_variabila     {$$ = OperatorFunction($1, "+", $3); if(ProgramError()) {return -1;}}
     | oper_variabila '-' oper_variabila     {$$ = OperatorFunction($1, "-", $3); if(ProgramError()) {return -1;}}
     | oper_variabila '*' oper_variabila     {$$ = OperatorFunction($1, "*", $3); if(ProgramError()) {return -1;}}
     | oper_variabila '/' oper_variabila     {$$ = OperatorFunction($1, "/", $3); if(ProgramError()) {return -1;}} 
     | oper_variabila '%' oper_variabila     {$$ = OperatorFunction($1, "%", $3); if(ProgramError()) {return -1;}}
     | '(' oper_variabila ')'       {$$ = $2;}
     | oper_variabila OP oper_variabila
     | ID            {$$ = GetVariable($1);}
     | INTEGER       {struct variable tempVar; strcpy(tempVar.varName, "tempVar"); tempVar.value.intVal = $1; tempVar.dataType = INT_t; 
                     tempVar.isInitialized = true; $$ = tempVar;}
     | FLOAT        {struct variable tempVar; strcpy(tempVar.varName, "tempVar"); tempVar.value.floatVal = $1; tempVar.dataType = FLOAT_t; 
                    tempVar.isInitialized = true; $$ = tempVar;}
     | STRING        {struct variable tempVar; tempVar.value.stringVal = (char*)malloc (strlen($1)+1); strcpy (tempVar.varName, "tempVar"); 
                     strcpy (tempVar.value.stringVal, $1); tempVar.dataType = STRING_t; tempVar.isInitialized = true;  $$ = tempVar;}
     ;

oper_asignare: ADD_ASSIGN {strcpy ($$, $1);}
         | SUB_ASSIGN {strcpy ($$, $1);}
         | MUL_ASSIGN {strcpy ($$, $1);}
         | DIV_ASSIGN {strcpy ($$, $1);}
         | MOD_ASSIGN {strcpy ($$, $1);}
         ;

apel_func: ID '(' lista_param_apel_func ')' {int auxDataType; FunctionCallWithParameters($1); auxDataType = GetFunction($1).dataType; nrParams = 0; if(ProgramError()) {return -1;} $$ = auxDataType;}
             | ID '('')'                           {FunctionCallNoParameters($1); if(ProgramError()) {return -1;} $$ = GetFunction($1).dataType;}
             ;

lista_param_apel_func: apel_func_param                              {parameters[nrParams] = $1; ++nrParams;}
                        | apel_func_param ',' lista_param_apel_func {parameters[nrParams] = $1; ++nrParams; }
                        ;

apel_func_param: ID         {struct variable var; var = GetVariable($1); if(ProgramError()) {return -1;} strcpy($$.parName, $1); $$.dataType = GetVariable($1).dataType; $$.isFunction = false;}
                   | INTEGER    {$$.dataType = INT_t; $$.isFunction = false;}
                   | FLOAT     {$$.dataType = FLOAT_t; $$.isFunction = false;}
                   | STRING     {$$.dataType = STRING_t; $$.isFunction = false;}
                   | apel_func  {$$.dataType = $1; $$.isFunction = true;}
                   ;

enunt_control:  enunt_if
                 | enunt_for
                 | enunt_while
                 ;

enunt_while: WHILE '(' oper_variabila ')' '{' instructiuni '}'
               | WHILE '(' oper_variabila ')' '{''}'
               ;

enunt_for: FOR '(' atrib_for '?' oper_variabila '?' oper_variabila ')'
             | FOR '(' atrib_for '?' oper_variabila '?' atrib_for ')' 
             | FOR '(' atrib_for '?' oper_variabila '?' ')'
             | FOR '(' '?' oper_variabila '?' oper_variabila ')'
             | FOR '(' '?' oper_variabila '?' atrib_for ')'
             | FOR '(' '?' oper_variabila '?' ')'
             | FOR '(' atrib_for '?' oper_variabila '?' oper_variabila ')' '{' instructiuni '}'
             | FOR '(' atrib_for '?' oper_variabila '?' atrib_for ')' '{' instructiuni '}'
             | FOR '(' atrib_for '?' oper_variabila '?' ')' '{' instructiuni '}'
             | FOR '(' '?' oper_variabila '?' oper_variabila ')' '{' instructiuni '}'
             | FOR '(' '?' oper_variabila '?' atrib_for ')' '{' instructiuni '}'
             | FOR '(' '?' oper_variabila '?' ')' '{' instructiuni '}' 
             ;

atrib_for: atribuire
               | atribuire ',' atrib_for
               ;

enunt_if: IF oper_variabila '{' instructiuni '}'
            | IF oper_variabila '{' '}'
            | IF oper_variabila '{' instructiuni '}' ELSE '{' instructiuni '}'
            | IF oper_variabila '{' '}' ELSE '{' instructiuni '}'
            | IF oper_variabila '{' instructiuni '}' ELSE '{' '}'
            | IF oper_variabila '{' '}' ELSE '{' '}'
            ;

%%
bool ProgramError() {
    if(haveError) {
        yyerror(errorMessage);
        return true; //Schimba in true pentru a opri programul cand gaseste o eroare
    }
    return false;
}

void yyerror(const char *s) {
    printf("eroare: %s la linia: %d\n", s, yylineno);
}

int main(int argc, char** argv) {
    yyin = fopen(argv[1], "r");
    yyparse();
    printSymbols();
}
