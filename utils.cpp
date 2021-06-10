#include "./utils.h"

int nrParams, nrVars, nrVectors, nrFunctions, nrCampuri, nrClasses, nrInstances, nextVarId, nextFunctionId, nextClassId;
bool haveError;
char errorMessage[MAX_ERROR], campuri[MAX_VARS][MAX_VAR_NAME];
bool varDeclared[MAX_VARS];
struct variable vars[MAX_VARS];
struct Vector vectors[MAX_VARS];
struct Function functions[MAX_VARS];
struct Class classes[MAX_VARS];
struct Parameter parameters[MAX_PARAMETERS];

void MallocMemoryForVector(struct Vector *);
bool AlreadyHaveVariableOrVectorName(char *);

void init() {
    nrVars = nrVectors = nrFunctions = nrCampuri = nrClasses = nrInstances = 0;
    nextVarId = nextFunctionId = nextClassId = 0;
    haveError = false;
    errorMessage[0] = 0;
    bzero(varDeclared, sizeof(varDeclared));
}

int SetDataType(const char *text) {
    if(strcmp(text, "int") == 0) return INT_t;
    if(strcmp(text, "float") == 0) return FLOAT_t;
    if(strcmp(text, "bells") == 0) return CHAR_t;
    if(strcmp(text, "tree") == 0) return STRING_t;
    if(strcmp(text, "elf") == 0) return BOOL_t;
    return INVALID_t;
}

void printSymbols()
{
    FILE *out = fopen("symbol_table.txt", "w");
    fprintf(out, "\nFunctii:\n");
    for(int i = 0; i<nrFunctions; i++)
    {
        fprintf(out, "%s | %d\n", functions[i].functionName, functions[i].dataType);
    }
    fprintf(out, "\nVariabile:\n");
    for(int i = 0; i<nrVars; i++)
    {
        fprintf(out, "%s | %d\n", vars[i].varName, vars[i].dataType);
    }
    fprintf(out, "\nVectori:\n");
    for(int i = 0; i<nrVectors; i++)
    {
        fprintf(out, "%s | %d | Marime:%d\n", vectors[i].vectorName, vectors[i].dataType, vectors[i].size);
    }
    fprintf(out, "\nClase:\n");
    for(int i = 0; i<nrVars; i++)
    {
        fprintf(out, "%s\n", classes[i].className);
    }
}

void AddNewVariable(int type, char *varName) {
    if(AlreadyHaveVariableOrVectorName(varName)) return;
    vars[nrVars].dataType = type;
    strcpy(vars[nrVars].varName, varName);
    vars[nrVars].idVar = ++nextVarId;
    vars[nrVars].isConstant = false;
    vars[nrVars].isInitialized = false;
    ++nrVars;
}

void AddNewVariableInit(int type, char *varName, int value) {
    if(AlreadyHaveVariableOrVectorName(varName)) return;
    vars[nrVars].dataType = type;
    strcpy(vars[nrVars].varName, varName);
    vars[nrVars].idVar = ++nextVarId;
    vars[nrVars].isConstant = false;
    vars[nrVars].isInitialized = true;
    ++nrVars;
    AssignValue(varName, value);
    if(haveError) --nrVars;
    return;
}

void AddNewVariableInit(int type, char *varName, float value) {
    if(AlreadyHaveVariableOrVectorName(varName)) return;
    vars[nrVars].dataType = type;
    strcpy(vars[nrVars].varName, varName);
    vars[nrVars].idVar = ++nextVarId;
    vars[nrVars].isConstant = false;
    vars[nrVars].isInitialized = true;
    ++nrVars;
    AssignValue(varName, value);
    if(haveError) --nrVars;
    return;
}

void AddNewVariableInit(int type, char *varName, char* value) {
    if(AlreadyHaveVariableOrVectorName(varName)) return;
    vars[nrVars].dataType = type;
    strcpy(vars[nrVars].varName, varName);
    vars[nrVars].idVar = ++nextVarId;
    vars[nrVars].isConstant = false;
    vars[nrVars].isInitialized = true;
    ++nrVars;
    AssignValue(varName, value);
    if(haveError) --nrVars;
    return;
}

void AddNewFunction(int dataType, char *fName) {
    bool good;
    if(AlreadyHaveVariableOrVectorName(fName)) {
        good = false;
        for(int i = 0; i < nrFunctions; ++i)
            if(strcmp(functions[i].functionName, fName) == 0) {
                if(functions[i].nrParams != 0) good = true;
                break;
            }
        if(!good) return;
        else haveError = false;
    }
    strcpy(functions[nrFunctions].functionName, fName);
    functions[nrFunctions].dataType = dataType;
    functions[nrFunctions].idFunction = nextFunctionId++;
    functions[nrFunctions].nrParams = 0;
    functions[nrFunctions].isDefined = false;
    ++nrFunctions;
}

void AddNewFunctionWithParameters(int dataType, char *fName) {
    bool good;
    if(AlreadyHaveVariableOrVectorName(fName)) {
        good = false;
        for(int i = 0; i < nrFunctions; ++i)
            if(strcmp(functions[i].functionName, fName) == 0) {
                if(functions[i].nrParams != nrParams) good = true;
                else for (int j = 0; j < nrParams; ++j)
                        if(parameters[j].dataType != functions[i].parameters[j]) good = true;
                break;
            }
        if(!good) return;
        else haveError = false;
    }
    strcpy(functions[nrFunctions].functionName, fName);
    functions[nrFunctions].dataType = dataType;
    functions[nrFunctions].idFunction = nextFunctionId++;
    functions[nrFunctions].nrParams = nrParams;
    functions[nrFunctions].isDefined = false;
    for(int i = 0; i < nrParams; ++i) functions[nrFunctions].parameters[i] = parameters[i].dataType;
    ++nrFunctions;
}

void AddNewClass(char *className) {
    if(AlreadyHaveVariableOrVectorName(className)) return;
    strcpy(classes[nrClasses].className, className);
    classes[nrClasses].idClass = nextClassId++;
    ++nrClasses;
}

bool AlreadyHaveVariableOrVectorName(char *name) {
    int i;
    for (i = 0; i < nrVectors; ++i) {
        if(strcmp(vectors[i].vectorName, name) == 0) {
            haveError = true;
            strcpy(errorMessage, "Error: vector already exists! Merry Christmas!");
            return true;
        }
    }
    for(i = 0; i < nrVars; ++i) {
        if (strcmp(vars[i].varName, name) == 0) {
            haveError = true;
            strcpy(errorMessage, "Error: variable already exists! Merry Christmas!");
            return true;
        }
    }
    for(i = 0; i < nrFunctions; ++i) {
        if(strcmp(functions[i].functionName, name) == 0) {
            haveError = true;
            strcpy(errorMessage, "Error: function already exists! Merry Christmas!");
            return true;
        }
    }
    for(i = 0; i < nrClasses; ++i) {
        if (strcmp(classes[i].className, name) == 0) {
            haveError = true;
            strcpy(errorMessage, "Error: class already exists! Merry Christmas!");
            return true;
        }
    }
    return false;
}


void DeclareVector(int vectorType, char *vName, int vSize) {
    if(AlreadyHaveVariableOrVectorName(vName)) return;
    strcpy(vectors[nrVectors].vectorName, vName);
    vectors[nrVectors].dataType = vectorType;
    vectors[nrVectors].size = vSize;
    vectors[nrVectors].idVector = nextVarId++;
    MallocMemoryForVector(&vectors[nrVectors]);
    ++nrVectors;
}

void MallocMemoryForVector(struct Vector *vector) {
    vector->values = (struct data *) malloc(vector->size * sizeof(struct data));
}

void AddNewConstant(int dataType, char *varName, int value) {
    if(AlreadyHaveVariableOrVectorName(varName)) return;
    vars[nrVars].dataType = dataType;
    strcpy(vars[nrVars].varName, varName);
    vars[nrVars].idVar = ++nextVarId;
    vars[nrVars].isInitialized = true;
    ++nrVars;
    AssignValue(varName, value);
    if(haveError) --nrVars;
    vars[nrVars-1].isConstant = true;
    return;
}

void AddNewConstant(int dataType, char *varName, float value) {
    if(AlreadyHaveVariableOrVectorName(varName)) return;
    vars[nrVars].dataType = dataType;
    strcpy(vars[nrVars].varName, varName);
    vars[nrVars].idVar = ++nextVarId;
    vars[nrVars].isInitialized = true;
    ++nrVars;
    AssignValue(varName, value);
    if(haveError) --nrVars;
    vars[nrVars-1].isConstant = true;
    return;
}

void AddNewConstant(int dataType, char *varName, char *strVal) {
    if(AlreadyHaveVariableOrVectorName(varName)) return;
    vars[nrVars].dataType = dataType;
    strcpy(vars[nrVars].varName, varName);
    vars[nrVars].idVar = ++nextVarId;
    vars[nrVars].isInitialized = true;
    ++nrVars;
    AssignValue(varName, strVal);
    if(haveError) --nrVars;
    vars[nrVars-1].isConstant = true;
    return;
}

void AssignValue(char *varName, int val) {
    int i;
    for(i = 0; i < nrVars; ++i)
        if(strcmp(varName, vars[i].varName) == 0) {
            if(vars[i].isConstant) {
                haveError = true;
                strcpy(errorMessage, "Error: can't reinitialize a constant variable! Merry Christmas!");
                return;
            }
            if(vars[i].dataType == BOOL_t)
                if(val != 0 && val != 1) {
                    haveError = true;
                    strcpy(errorMessage, "Error: wrong value for bool! Merry Christmas!");
                    return;
                }
                else {
                    vars[i].value.boolVal = val;
                    vars[i].isInitialized = true;
                }
            if(vars[i].dataType == STRING_t) {
                haveError = true;
                strcpy(errorMessage, "Error: wrong assignment! Merry Christmas!");
                return;
            }
            switch (vars[i].dataType) {
            case FLOAT_t:
                vars[i].value.floatVal = val;
                vars[i].isInitialized = true;
                break;
            case INT_t:
                vars[i].value.intVal = val;
                vars[i].isInitialized = true;
                break;
            }
            return;
        }
    return;
}

void AssignValue(char *varName, float val) {
    int i;
    for(i = 0; i < nrVars; ++i)
        if(strcmp(varName, vars[i].varName) == 0) {
            if(vars[i].isConstant) {
                haveError = true;
                strcpy(errorMessage, "Error: can't reinitialize a constant variable! Merry Christmas!");
                return;
            }
            if(vars[i].dataType != FLOAT_t) {
                haveError = true;
                strcpy(errorMessage, "Error: wrong assignment! Merry Christmas!");
                return;
            }
            vars[i].value.floatVal = val;
            vars[i].isInitialized = true;
            return;
        }
    return;
}

void AssignValue(char *varName, char *strVal) {
    int i;
    for(i = 0; i < nrVars; ++i)
        if(strcmp(varName, vars[i].varName) == 0) {
            if (vars[i].isConstant) {
                haveError = true;
                strcpy(errorMessage, "Error: can't reinitialize a constant variable! Merry Christmas!");
                return;
            }
            if (vars[i].dataType != CHAR_t && vars[i].dataType != STRING_t) {
                haveError = true;
                strcpy(errorMessage, "Error: wrong assignment! Merry Christmas!");
                return;
            }
            switch (vars[i].dataType) {
            case CHAR_t:
                if(strlen(strVal) > 1) {
                    haveError = true;
                    strcpy(errorMessage, "Error: can't assign string to char! Merry Christmas!");
                    return;
                }
                vars[i].value.charVal = strVal[0];
                vars[i].isInitialized = true;
                break;
            case STRING_t:
                if(vars[i].value.stringVal != NULL) delete vars[i].value.stringVal;
                vars[i].value.stringVal = new char[strlen(strVal)+1];
                strcpy(vars[i].value.stringVal, strVal);
                vars[i].isInitialized = true;
                break;
            }
            return;
        }
}

void Eval(struct variable var) {
    if(var.dataType != INT_t)
    {
        haveError = true;
        strcpy(errorMessage, "Error: parameter must be of type int! Merry Christmas!");
    }
    else
        printf("%d\n", var.value.intVal);
}

struct variable GetVariable(char *varName) {
    for (int i = 0; i < nrVars; ++i)
        if (strcmp(vars[i].varName, varName) == 0) return vars[i];
    haveError = true;
    strcpy(errorMessage, "Error: variable doesn't exist! Merry Christmas!");
    return vars[0];
}

void AssignVarValue(char *varName1, struct variable var2) {
    int i = -1;
    for(i = 0; i < nrVars; ++i)
        if(strcmp(varName1, vars[i].varName) == 0) break;
    if(i == nrVars) {
        haveError = true;
        strcpy(errorMessage, "Error: variable doesn't exist! Merry Christmas!");
        return;
    }
    switch (var2.dataType) {
    case INT_t:
        AssignValue(varName1, var2.value.intVal);
        break;
    case FLOAT_t:
        AssignValue(varName1, var2.value.floatVal);
        break;
    case CHAR_t:
        char strTemp[2];
        strTemp[0] = var2.value.charVal;
        strTemp[1] = 0;
        AssignValue(varName1, strTemp);
        break;
    case STRING_t:
        AssignValue(varName1, var2.value.stringVal);
        break;
    case BOOL_t:
        AssignValue(varName1, (int)var2.value.boolVal);
        break;
    }
}

void AssignVectorValue(char *vecName, int pos, struct variable var) {
    int i;
    for(i = 0; i < nrVectors; ++i)
        if(strcmp(vecName, vectors[i].vectorName) == 0) break;
    if(i == nrVectors) {
        haveError = true;
        strcpy(errorMessage, "Error: vector doesn't exist! Merry Christmas!");
        return;
    }
    switch (var.dataType) {
    case INT_t:
        AssignVecValue(vecName, pos, var.value.intVal);
        break;
    case FLOAT_t:
        AssignVecValue(vecName, pos, var.value.floatVal);
        break;
    case CHAR_t:
        char strTemp[2];
        strTemp[0] = var.value.charVal;
        strTemp[1] = 0;
        AssignVecValue(vecName, pos, strTemp);
        break;
    case STRING_t:
        AssignVecValue(vecName, pos, var.value.stringVal);
        break;
    case BOOL_t:
        AssignVecValue(vecName, pos, (int)var.value.boolVal);
        break;
    }
}

void AssignVecValue(char *vecName, int pos, int val) {
    int i;
    for(i = 0; i < nrVectors; ++i)
        if(strcmp(vecName, vectors[i].vectorName) == 0) {
            if(vectors[i].dataType == BOOL_t)
                if(val != 0 && val != 1) {
                    haveError = true;
                    strcpy(errorMessage, "wrong value for bool");
                    return;
                }
                else {
                    vectors[i].values[pos].boolVal = val;
                }
            if (vectors[i].dataType == STRING_t) {
                haveError = true;
                strcpy(errorMessage, "Error: wrong assignment! Merry Christmas!");
                return;
            }
            switch (vectors[i].dataType) {
            case FLOAT_t:
                vectors[i].values[pos].floatVal = val;
                break;
            case INT_t:
                vectors[i].values[pos].intVal = val;
                break;
            }
            return;
        }
    return;
}

void AssignVecValue(char *vecName, int pos, float val) {
    int i;
    for(i = 0; i < nrVectors; ++i)
        if(strcmp(vecName, vectors[i].vectorName) == 0) {
            if (vectors[i].dataType != FLOAT_t) {
                haveError = true;
                strcpy(errorMessage, "Error: wrong assignment! Merry Christmas!");
                return;
            }
            vectors[i].values[pos].floatVal = val;
            return;
        }
    return;
}

void AssignVecValue(char *vecName, int pos, char *strVal) {
    int i;
    for(i = 0; i < nrVectors; ++i)
        if(strcmp(vecName, vectors[i].vectorName) == 0) {
            if(vectors[i].dataType != CHAR_t && vectors[i].dataType != STRING_t) {
                haveError = true;
                strcpy(errorMessage, "dude, wrong assignment... ugh...");
                return;
            }
            switch (vectors[i].dataType) {
            case CHAR_t:
                if (strlen(strVal) > 1) {
                    haveError = true;
                    strcpy(errorMessage, "can't assign string to char!");
                    return;
                }
                vectors[i].values[pos].charVal = strVal[0];
                break;
            case STRING_t:
                if(vectors[i].values[pos].stringVal != NULL) delete vectors[i].values[pos].stringVal;
                vectors[i].values[pos].stringVal = new char[strlen(strVal) + 1];
                strcpy(vectors[i].values[pos].stringVal, strVal);
                break;
            }
            return;
        }
}

void FunctionCallNoParameters(char *fId) {
    Function f;
    f = GetFunction(fId);
    if(haveError) return;
    if(f.nrParams != 0) {
        haveError = true;
        strcpy(errorMessage, "Error: function needs to receive parameters! Merry Christmas!");
        return;
    }
}

void FunctionCallWithParameters(char *fId) {
    Function f;
    f = GetFunction(fId);
    if(haveError) return;
    if (nrParams != f.nrParams) {
        haveError = true;
        strcpy(errorMessage, "Error: wrong number of parameters! Merry Christmas!");
        return;
    }
    for(int i = 0; i < f.nrParams; ++i) {
        if(f.parameters[i] != parameters[i].dataType) {
            haveError = true;
            strcpy(errorMessage, "Error: parameters have different types! Merry Christmas!");
            return;
        }
    }
}

struct Function GetFunction(char *fName) {
    bool good;
    for(int i = 0; i < nrFunctions; ++i) {
        good = true;
        if(strcmp(functions[i].functionName, fName) == 0 && nrParams == functions[i].nrParams) {
            for(int j = 0; j < nrParams; ++j)
                if(parameters[j].dataType != functions[i].parameters[j]) good = false;
            if(good) return functions[i];
        }
    }
    haveError = true;
    strcpy(errorMessage, "Error: this function doesn't exist! Merry Christmas!");
    return functions[0];
}

struct variable OperatorFunction(struct variable var1, const char *op, struct variable var2) {
    struct variable rez;
    rez.isInitialized = false;

    if(var1.isInitialized == false || var2.isInitialized == false) {
        haveError = true;
        strcpy(errorMessage, "Error: variable uninitialized! Merry Christmas!");
        return rez;
    }
    if (strcmp(op, "+=") == 0) rez = SumFunction(var1, var2);
    if (strcmp(op, "+") == 0) rez = SumFunction(var1, var2);
    if (strcmp(op, "-") == 0) rez = MinusFunction(var1, var2);
    if (strcmp(op, "-=") == 0) rez = MinusFunction(var1, var2);
    if (strcmp(op, "*") == 0) rez = TimesFunction(var1, var2);
    if (strcmp(op, "*=") == 0) rez = TimesFunction(var1, var2);
    if (strcmp(op, "/") == 0) rez = FractionFunction(var1, var2);
    if (strcmp(op, "/=") == 0) rez = FractionFunction(var1, var2);
    if (strcmp(op, "%") == 0) rez = ModFunction(var1, var2);
    if (strcmp(op, "%=") == 0) rez = ModFunction(var1, var2);
    rez.isInitialized = true;
    return rez;
}

struct variable SumFunction(struct variable a, struct variable b) {
    struct variable res;
    if(a.dataType == INT_t && b.dataType == INT_t) {
        res.value.intVal = a.value.intVal + b.value.intVal;
        res.dataType = INT_t;
    }
    if(a.dataType == FLOAT_t && b.dataType == FLOAT_t) {
        res.value.floatVal = a.value.floatVal + b.value.floatVal;
        res.dataType = FLOAT_t;
    }
    if(a.dataType == FLOAT_t && b.dataType == INT_t) {
        res.value.floatVal = a.value.floatVal + b.value.intVal;
        res.dataType = FLOAT_t;
    }
    if(a.dataType == INT_t && b.dataType == FLOAT_t) res = SumFunction(b, a);

    if(a.dataType == CHAR_t && b.dataType == CHAR_t) {
        res.value.stringVal = (char *)malloc(3);
        res.value.stringVal[0] = a.value.charVal;
        res.value.stringVal[1] = b.value.charVal;
        res.value.stringVal[2] = 0;
        res.dataType = STRING_t;
    }
    if(a.dataType == STRING_t && b.dataType == CHAR_t) {
        res.value.stringVal = (char *)malloc(strlen(a.value.stringVal) + 2);
        strcpy(res.value.stringVal, a.value.stringVal);
        res.value.stringVal[strlen(res.value.stringVal)] = b.value.charVal;
        res.value.stringVal[sizeof(res.value.stringVal) - 1] = 0;
        res.dataType = STRING_t;
    }
    if(a.dataType == CHAR_t && b.dataType == STRING_t) res = SumFunction(b, a);

    if(a.dataType == STRING_t && b.dataType == STRING_t) {
        res.value.stringVal = (char *)malloc(strlen(a.value.stringVal) + strlen(b.value.stringVal) + 1);
        strcpy(res.value.stringVal, a.value.stringVal);
        strcat(res.value.stringVal, b.value.stringVal);
        res.dataType = STRING_t;
    }
    return res;
}

struct variable MinusFunction(struct variable a, struct variable b) {
    struct variable res;
    if(a.dataType == INT_t && b.dataType == INT_t) {
        res.value.intVal = a.value.intVal - b.value.intVal;
        res.dataType = INT_t;
    }
    if(a.dataType == FLOAT_t && b.dataType == FLOAT_t) {
        res.value.floatVal = a.value.floatVal - b.value.floatVal;
        res.dataType = FLOAT_t;
    }
    if(a.dataType == FLOAT_t && b.dataType == INT_t) {
        res.value.floatVal = a.value.floatVal - b.value.intVal;
        res.dataType = FLOAT_t;
    }
    if(a.dataType == INT_t && b.dataType == FLOAT_t) {
        res.value.floatVal = a.value.intVal - b.value.floatVal;
        res.dataType = FLOAT_t;
    }
    return res;
}

struct variable TimesFunction(struct variable a, struct variable b) {
    struct variable res;
    if(a.dataType == INT_t && b.dataType == INT_t) {
        res.value.intVal = a.value.intVal * b.value.intVal;
        res.dataType = INT_t;
    }
    if(a.dataType == FLOAT_t && b.dataType == FLOAT_t) {
        res.value.floatVal = a.value.floatVal * b.value.floatVal;
        res.dataType = FLOAT_t;
    }
    if(a.dataType == FLOAT_t && b.dataType == INT_t) {
        res.value.floatVal = a.value.floatVal * b.value.intVal;
        res.dataType = FLOAT_t;
    }
    if(a.dataType == INT_t && b.dataType == FLOAT_t) res = TimesFunction(b, a);
    return res;
}

struct variable FractionFunction(struct variable a, struct variable b) {
    struct variable res;
    if(a.dataType == INT_t && b.dataType == INT_t) {
        res.value.intVal = a.value.intVal / b.value.intVal;
        res.dataType = INT_t;
    }
    if(a.dataType == FLOAT_t && b.dataType == FLOAT_t) {
        res.value.floatVal = a.value.floatVal / b.value.floatVal;
        res.dataType = FLOAT_t;
    }
    if(a.dataType == FLOAT_t && b.dataType == INT_t) {
        res.value.floatVal = a.value.floatVal / b.value.intVal;
        res.dataType = FLOAT_t;
    }
    if(a.dataType == INT_t && b.dataType == FLOAT_t) {
        res.value.floatVal = a.value.intVal / b.value.floatVal;
        res.dataType = FLOAT_t;
    }
    return res;
}

struct variable ModFunction(struct variable a, struct variable b) {
    struct variable res;
    if(a.dataType == INT_t && b.dataType == INT_t) {
        res.value.intVal = a.value.intVal % b.value.intVal;
        res.dataType = INT_t;
    }
    return res;
}
