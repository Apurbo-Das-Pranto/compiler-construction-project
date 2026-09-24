#pragma once

#include <iostream>
#include <string>

using namespace std;

class SymbolInfo {
private:
    string name;
    string type;
    
  
    string dataType;       
    bool isFunction;       
    bool isDefined;        
    bool isArray;          
    int arraySize;        
    

    bool isGlobal;         
    int offset;            

public:
    SymbolInfo* next;

    SymbolInfo(string name, string type) {
        this->name = name;
        this->type = type;
        this->next = nullptr;
        this->dataType = "";
        this->isFunction = false;
        this->isDefined = false;
        this->isArray = false;
        this->arraySize = 0;
        this->isGlobal = false;
        this->offset = 0;
    }

    SymbolInfo(string name, string type, string dataType) {
        this->name = name;
        this->type = type;
        this->next = nullptr;
        this->dataType = dataType;
        this->isFunction = false;
        this->isDefined = false;
        this->isArray = false;
        this->arraySize = 0;
        this->isGlobal = false;
        this->offset = 0;
    }

    SymbolInfo* getNext() { return next; }
    void setNext(SymbolInfo* next) { this->next = next; }   
    string getName() const { return name; }
    string getType() const { return type; }
    string getDataType() const { return dataType; }
    bool getIsFunction() const { return isFunction; }
    bool getIsDefined() const { return isDefined; }
    bool getIsArray() const { return isArray; }
    int getArraySize() const { return arraySize; }
    bool getIsGlobal() const { return isGlobal; }
    int getOffset() const { return offset; }


    void setName(string name) { this->name = name; }
    void setType(string type) { this->type = type; }
    void setDataType(string dataType) { this->dataType = dataType; }
    void setIsFunction(bool isFunction) { this->isFunction = isFunction; }
    void setIsDefined(bool isDefined) { this->isDefined = isDefined; }
    void setIsArray(bool isArray) { this->isArray = isArray; }
    void setArraySize(int arraySize) { this->arraySize = arraySize; }
    void setIsGlobal(bool isGlobal) { this->isGlobal = isGlobal; }
    void setOffset(int offset) { this->offset = offset; }
};