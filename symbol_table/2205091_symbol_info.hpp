#ifndef _2205091_SYMBOL_INFO_HPP
#define _2205091_SYMBOL_INFO_HPP

//#include<bits/stdc++.h>
#include<string>
using namespace std;

class SymbolInfo {
    // Class definition here
    private:
        string name;
        string type;
        SymbolInfo* next;

    public:
        
        SymbolInfo(string name="",string type="") {
            this->name = name;
            this->type = type;
            this->next = nullptr;
        }
        ~SymbolInfo(){
        }
        string getName() {
            return name;
        }
        string getType() {
            return type;
        }
        SymbolInfo* getNext() {
            return next;
        }
        void setName(string name) {
            this->name = name;
        }           
        void setType(string type) {
            this->type = type;
        }
        void setNext(SymbolInfo* next) {
            this->next = next;
        }
};

#endif // _2205091_SYMBOL_INFO_HPP