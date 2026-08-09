#ifndef _2205091_SYMBOL_TABLE_HPP
#define _2205091_SYMBOL_TABLE_HPP

#include<iostream>
#include<string>
#include "2205091_scope_table.hpp"

using namespace std;

class SymbolTable{
    private:
        ScopeTable * currentScopeTable;
        int bucketSize;
        int uniqueNumber;
    public:
        SymbolTable(int bucketSize){
            this->bucketSize = bucketSize;
            this->uniqueNumber = 1;
            this->currentScopeTable = new ScopeTable(bucketSize, uniqueNumber, nullptr);
        }
        ~SymbolTable(){
            while(currentScopeTable!=nullptr){
                ScopeTable * temp = currentScopeTable;
                currentScopeTable = currentScopeTable->getParentScopeTable();
                delete temp;
            }
        }
        ScopeTable* getCurrentScopeTable(){
            return currentScopeTable;
        }


        void enterScope(){
            uniqueNumber++;
            ScopeTable * newScopeTable = new ScopeTable(bucketSize, uniqueNumber, currentScopeTable);
            currentScopeTable = newScopeTable;
        }
        void exitScope(bool silent=false){
            // main scope table should not be deleted
            if(currentScopeTable->getParentScopeTable()!=nullptr){
                ScopeTable * temp = currentScopeTable;
                currentScopeTable = currentScopeTable->getParentScopeTable();
                if(!silent){
                    cout<<"\tScopeTable# "<<temp->getUniqueNumber()<<" removed"<<endl;
                }
                delete temp;
                // uniqueNumber--;
            }

        }
        bool insertSymbolInfo(string name,string type){
            return currentScopeTable->insertSymbolInfo(name, type);
        }
        bool removeSymbolInfo(string name){
            return currentScopeTable->deleteSymbolInfo(name);
        }
        SymbolInfo* lookUpSymbolInfo(string name){
            ScopeTable *temp=currentScopeTable;
            while(temp!=nullptr){
                ScopeTable *parentScopeTable=temp->getParentScopeTable();
                SymbolInfo *symbolInfo=temp->lookUpSymbolInfo(name);
                if(symbolInfo!=nullptr){
                    return symbolInfo;  
                }
                temp=parentScopeTable;
            }
            return nullptr;
        }
        void printCurrentScopeTable(){
            int tab=1;
            currentScopeTable->printScopeTable(tab);
            cout<<endl;
        }
        void printAllScopeTable(){
            int tab=1;
            ScopeTable *temp=currentScopeTable;
            while(temp!=nullptr){
                temp->printScopeTable(tab);
                temp=temp->getParentScopeTable();
                tab++;
            }
            cout<<endl;
        }
        void deleteAllScopeTable(){
            while(currentScopeTable!=nullptr){
                ScopeTable * temp = currentScopeTable;
                currentScopeTable = currentScopeTable->getParentScopeTable();
                cout<<"\tScopeTable# "<<temp->getUniqueNumber()<<" removed"<<endl;
                delete temp;   
            }
        }
            
};

#endif // _2205091_SYMBOL_TABLE_HPP