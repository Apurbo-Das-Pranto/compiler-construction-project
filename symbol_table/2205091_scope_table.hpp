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
        ScopeTable* parentScopeTable;
    public:
        ScopeTable(int bucketSize, int uniqueNumber, ScopeTable* parentScopeTable){
            this->bucketSize = bucketSize;
            this->uniqueNumber = uniqueNumber;
            this->parentScopeTable = parentScopeTable;
            hashTable = new SymbolInfo*[bucketSize];
            for(int i=0;i<bucketSize;i++){
                hashTable[i] = nullptr;
            }
            cout<<"\tScopeTable# "<<uniqueNumber<<" created"<<endl;
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

        unsigned int SDBMHash(string str , unsigned int num_buckets) {
            unsigned int hash = 0;
            unsigned int len = str.length ();

            for (unsigned int i = 0; i < len; i++)
            {
            hash = ((str[i]) + (hash << 6) + (hash << 16) - hash) % num_buckets;
            }

            return hash;
        }
        
        bool insertSymbolInfo(string name,string type){
            int bucketNumber = SDBMHash(name, bucketSize);
            if(hashTable[bucketNumber]==nullptr){
                hashTable[bucketNumber] = new SymbolInfo(name, type);
                cout<<"\tInserted in ScopeTable# "<<uniqueNumber<<" at position "<<bucketNumber+1<<", 1"<<endl;
                return true;
            }
            else{
                SymbolInfo * currentSymbolInfo=hashTable[bucketNumber];
                SymbolInfo * previousSymbolInfo=nullptr;
                int position = 1;
                while(currentSymbolInfo!=nullptr){
                    if(currentSymbolInfo->getName()==name){
                        //'<=' already exists in the current ScopeTable
                        cout<<"\t'"<<name<<"' already exists in the current ScopeTable"<<endl;
                        return false;
                    }
                    previousSymbolInfo = currentSymbolInfo;
                    currentSymbolInfo = currentSymbolInfo->getNext();
                    position++;
                }
                previousSymbolInfo->setNext(new SymbolInfo(name, type));
                cout<<"\tInserted in ScopeTable# "<<uniqueNumber<<" at position "<<bucketNumber+1<<", "<<position<<endl;
                return true;
            }
        }
        SymbolInfo*lookUpSymbolInfo(string name){
            int bucketNumber = SDBMHash(name, bucketSize);
            SymbolInfo * currentSymbolInfo=hashTable[bucketNumber];
            int position = 1;
            while(currentSymbolInfo!=nullptr){
                if(currentSymbolInfo->getName()==name){
                    //'i' found in ScopeTable# 1 at position 1, 1
                    cout<<"\t'"<<name<<"' found in ScopeTable# "<<uniqueNumber<<" at position "<<bucketNumber+1<<", "<<position<<endl;
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
                    cout<<"\tDeleted '"<<name<<"' from ScopeTable# "<<uniqueNumber<<" at position "<<bucketNumber+1<<", "<<position<<endl;
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
            cout<<string(tab,'\t')<<"ScopeTable# "<<uniqueNumber<<endl;
            for(int i=0;i<bucketSize;i++){
                cout<<string(tab,'\t')<<i+1<<"--> ";
                SymbolInfo * currentSymbolInfo=hashTable[i];
                while(currentSymbolInfo!=nullptr){
                    cout<<"<"<<currentSymbolInfo->getName()<<","<<currentSymbolInfo->getType()<<"> ";
                    currentSymbolInfo = currentSymbolInfo->getNext();
                }
                cout<<endl;  
            }
        }
};

#endif // _2205091_SCOPE_TABLE_HPP

