#ifndef _2205091_SYMBOL_INFO_HPP
#define _2205091_SYMBOL_INFO_HPP

#include <string>
#include <vector>
using namespace std;

struct Parameter {
    string name;
    string type; // "INT", "FLOAT", etc.
};

class SymbolInfo {
private:
    string name;
    string type;          // "ID", "CONST_INT", etc.
    string dataType;      // "INT", "FLOAT", "VOID"
    
    bool isArray = false;
    int arraySize = 0;
    
    bool isFunction = false;
    bool isDefined = false; // Function Definition vs Declaration check
    vector<Parameter> paramList;
    
    SymbolInfo* next;

public:
    SymbolInfo(string name = "", string type = "") {
        this->name = name;
        this->type = type;
        this->next = nullptr;
    }
    
    string getName() { return name; }
    string getType() { return type; }
    SymbolInfo* getNext() { return next; }
    void setName(string name) { this->name = name; }
    void setType(string type) { this->type = type; }
    void setNext(SymbolInfo* next) { this->next = next; }

    // Semantic Extensions
    string getDataType() { return dataType; }
    void setDataType(string dt) { dataType = dt; }

    bool getIsArray() { return isArray; }
    void setIsArray(bool arr) { isArray = arr; }

    int getArraySize() { return arraySize; }
    void setArraySize(int sz) { arraySize = sz; }

    bool getIsFunction() { return isFunction; }
    void setIsFunction(bool fn) { isFunction = fn; }

    bool getIsDefined() { return isDefined; }
    void setIsDefined(bool def) { isDefined = def; }

    vector<Parameter>& getParamList() { return paramList; }
    void addParameter(string pName, string pType) {
        paramList.push_back({pName, pType});
    }
};

#endif