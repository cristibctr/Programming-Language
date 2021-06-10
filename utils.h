#ifndef UTILS_H
#define UTILS_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#define MAX_VARS 1000
#define MAX_VAR_NAME 256
#define MAX_ERROR 1000
#define MAX_STRVAL 10000
#define MAX_PARAMETERS 100

#define BOOL_t -1
#define CHAR_t -2
#define INT_t -3
#define FLOAT_t -4
#define STRING_t -5
#define CLASS_t -6
#define INVALID_t -7

struct data {
  int intVal;
  float floatVal;
  bool boolVal;
  char charVal;
  char *stringVal;
};

struct variable {
  int dataType, idVar;
  char varName[MAX_VAR_NAME];
  bool isConstant, isInitialized;
  struct data value;
};

struct Parameter {
  char parName[MAX_VAR_NAME];
  int dataType;
  bool isFunction;
};

struct Function {
  int dataType, idFunction;
  char functionName[MAX_VAR_NAME];
  int parameters[MAX_PARAMETERS], nrParams;
  bool isDefined;
};

struct Vector {
  char vectorName[MAX_VAR_NAME];
  int dataType, idVector;
  int size;
  struct data *values;
};

struct Class {
  char className[MAX_VAR_NAME];
  int idClass;
};

extern bool haveError;
extern char errorMessage[MAX_ERROR];
extern struct Parameter parameters[MAX_PARAMETERS];
extern int nrParams;
extern char campuri[MAX_VARS][MAX_VAR_NAME];
extern int nrCampuri;

void printSymbols();

void init();
int SetDataType(const char *);

void AddNewVariable(int, char *);
void AddNewVariableInit(int, char *, int);
void AddNewVariableInit(int, char *, float);
void AddNewVariableInit(int, char *, char* );
void AddNewFunctionWithParameters(int, char *);
void AddNewFunction(int, char *);
void AddNewClass(char *);

struct variable GetVariable(char *);

void DeclareVector(int, char *, int);

void AssignValue(char *, int);
void AssignValue(char *, float);
void AssignValue(char *, char *);
void AssignVarValue(char *, struct variable);
void AssignVectorValue(char *vecName, int pos, struct variable);
void AssignVecValue(char *vecName, int pos, char *strVal);
void AssignVecValue(char *vecName, int pos, float val);
void AssignVecValue(char *vecName, int pos, int val);

void AddNewConstant(int, char *, int);
void AddNewConstant(int, char *, float);
void AddNewConstant(int, char *, char *);

void Eval(struct variable);

void FunctionCallNoParameters(char *);
void FunctionCallWithParameters(char *);
struct Function GetFunction(char *);

struct variable OperatorFunction(struct variable, const char *, struct variable);
struct variable SumFunction(struct variable, struct variable);
struct variable MinusFunction(struct variable, struct variable);
struct variable TimesFunction(struct variable, struct variable);
struct variable FractionFunction(struct variable, struct variable);
struct variable ModFunction(struct variable a, struct variable b);

#endif