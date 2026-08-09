#ifndef _2205091_SCOPE_TABLE_HPP
#define _2205091_SCOPE_TABLE_HPP

#include<iostream>
#include<string>
#include "2205091_symbol_info.hpp"
using namespace std;



class ScopeTable{
        SymbolInfo** hashTable;
        int uniqueNumber;
        int bucketSize;
        int childCounter;
        int labelNumber;
        ScopeTable* parentScopeTable;
    public:
        ScopeTable(int bucketSize, int uniqueNumber, ScopeTable* parentScopeTable){
            this->bucketSize = bucketSize;
            this->uniqueNumber = uniqueNumber;
            this->parentScopeTable = parentScopeTable;
            this->childCounter = 0;
            this->labelNumber = 1;
            hashTable = new SymbolInfo*[bucketSize];
            for(int i=0;i<bucketSize;i++){
                hashTable[i] = nullptr;
            }
        }
        ~ScopeTable(){
            for(int i=0;i<bucketSize;i++){
                 if(hashTable[i]!=nullptr){
                    SymbolInfo*currentSymbolInfo=hashTable[i];
                    while(currentSymbolInfo!=nullptr){
                        SymbolInfo*temp=currentSymbolInfo;
                        currentSymbolInfo=currentSymbolInfo->getNext();
                        delete temp;
                    }
                 }
            }
            delete[] hashTable;
        }
        ScopeTable* getParentScopeTable(){
            return parentScopeTable;
        }
        int getUniqueNumber(){
            return uniqueNumber;
        }
        void setLabelNumber(int value){
            labelNumber = value;
        }
        int getNextChildNumber(){
            return ++childCounter;
        }
        string getScopeLabel(){
            if(parentScopeTable == nullptr){
                return to_string(uniqueNumber);
            }
            return parentScopeTable->getScopeLabel() + "." + to_string(labelNumber);
        }

        unsigned int SDBMHash(string str, int bucketSize) {
            unsigned int hash = 0;
            auto *p = (unsigned char *) str.c_str();
            int c;
            while ((c = *p++)) {
                hash = c + (hash << 6) + (hash << 16) - hash;
            }
            return hash % bucketSize;
        }
        
        bool insertSymbolInfo(string name,string type){
            int bucketNumber = SDBMHash(name, bucketSize);
            if(hashTable[bucketNumber]==nullptr){
                hashTable[bucketNumber] = new SymbolInfo(name, type);
                return true;
            }
            else{
                SymbolInfo * currentSymbolInfo=hashTable[bucketNumber];
                SymbolInfo * previousSymbolInfo=nullptr;
                int position = 0;
                while(currentSymbolInfo!=nullptr){
                    if(currentSymbolInfo->getName()==name){
                        cout<<"< "<<name<<" : "<<type<<" > already exists in ScopeTable# "<<getScopeLabel()<<" at position "<<bucketNumber<<", "<<position<<endl<<endl;
                        return false;
                    }
                    previousSymbolInfo = currentSymbolInfo;
                    currentSymbolInfo = currentSymbolInfo->getNext();
                    position++;
                }
                previousSymbolInfo->setNext(new SymbolInfo(name, type));
                return true;
            }
        }
        SymbolInfo*lookUpSymbolInfo(string name){
            int bucketNumber = SDBMHash(name, bucketSize);
            SymbolInfo * currentSymbolInfo=hashTable[bucketNumber];
            int position = 0;
            while(currentSymbolInfo!=nullptr){
                if(currentSymbolInfo->getName()==name){
                    //'i' found in ScopeTable# 1 at position 1, 1
                    cout<<"\t'"<<name<<"' found in ScopeTable# "<<uniqueNumber<<" at position "<<bucketNumber<<", "<<position<<endl;
                    return currentSymbolInfo;
                }
                currentSymbolInfo = currentSymbolInfo->getNext();
                position++;
            }
            return nullptr;
        }
        bool deleteSymbolInfo(string name){
            int bucketNumber=SDBMHash(name,bucketSize);
            SymbolInfo *currentSymbolInfo=hashTable[bucketNumber];
            SymbolInfo *previousSymbolInfo=nullptr;
            int position=1;
            while(currentSymbolInfo!=nullptr){
                if(currentSymbolInfo->getName()==name){
                    if(previousSymbolInfo==nullptr){
                        hashTable[bucketNumber]=currentSymbolInfo->getNext();
                    }
                    else{
                        previousSymbolInfo->setNext(currentSymbolInfo->getNext());
                    }
                    delete currentSymbolInfo;
                    //Cmd 27: D ==
                    // Deleted '==' from ScopeTable# 5 at position 2, 1
                    return true;
                }
                previousSymbolInfo = currentSymbolInfo;
                currentSymbolInfo = currentSymbolInfo->getNext();
                position++;
            }
            cout<<"\tNot found in the current ScopeTable"<<endl;
            return false;
        }
        void printScopeTable(int tab=1){
            cout<<"ScopeTable # "<<getScopeLabel()<<endl;
            for(int i=0;i<bucketSize;i++){
                SymbolInfo * currentSymbolInfo=hashTable[i];
                if(currentSymbolInfo==nullptr){
                    continue;
                }
                cout<<i<<" --> ";
                while(currentSymbolInfo!=nullptr){
                    cout<<"< "<<currentSymbolInfo->getName()<<" : "<<currentSymbolInfo->getType()<<" >";
                    currentSymbolInfo = currentSymbolInfo->getNext();
                }
                cout<<endl;  
            }
        }
};

#endif // _2205091_SCOPE_TABLE_HPP