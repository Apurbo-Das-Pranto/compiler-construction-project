#ifndef _2205091_SCOPE_TABLE_HPP
#define _2205091_SCOPE_TABLE_HPP

#include <iostream>
#include <string>
#include "2205091_symbol_info.hpp"

using namespace std;

class ScopeTable
{
private:
    SymbolInfo **hashTable;
    int uniqueNumber;
    int bucketSize;
    int childCounter;
    int labelNumber;
    ScopeTable *parentScopeTable;

public:
    ScopeTable(int bucketSize, int uniqueNumber, ScopeTable *parentScopeTable)
    {
        this->bucketSize = bucketSize;
        this->uniqueNumber = uniqueNumber;
        this->parentScopeTable = parentScopeTable;
        this->childCounter = 0;
        this->labelNumber = 1;
        hashTable = new SymbolInfo *[bucketSize];
        for (int i = 0; i < bucketSize; i++)
        {
            hashTable[i] = nullptr;
        }
    }

    ~ScopeTable()
    {
        for (int i = 0; i < bucketSize; i++)
        {
            if (hashTable[i] != nullptr)
            {
                SymbolInfo *currentSymbolInfo = hashTable[i];
                while (currentSymbolInfo != nullptr)
                {
                    SymbolInfo *temp = currentSymbolInfo;
                    currentSymbolInfo = currentSymbolInfo->getNext();
                    delete temp;
                }
            }
        }
        delete[] hashTable;
    }

    ScopeTable *getParentScopeTable() { return parentScopeTable; }
    int getUniqueNumber() { return uniqueNumber; }
    void setLabelNumber(int value) { labelNumber = value; }
    int getNextChildNumber() { return ++childCounter; }

    string getScopeLabel()
    {
        if (parentScopeTable == nullptr)
        {
            return to_string(uniqueNumber);
        }
        return parentScopeTable->getScopeLabel() + "." + to_string(labelNumber);
    }

    unsigned int SDBMHash(string str, int bucketSize)
    {
        unsigned int hash = 0;
        for (char c : str)
        {
            hash += (unsigned char)c;
        }
        return hash % bucketSize;
    }

    SymbolInfo *insertSymbolInfo(SymbolInfo *newInfo)
    {
        if (newInfo == nullptr)
            return nullptr;
        string name = newInfo->getName();
        int bucketNumber = SDBMHash(name, bucketSize);

        if (hashTable[bucketNumber] == nullptr)
        {
            hashTable[bucketNumber] = newInfo;
            return newInfo;
        }
        else
        {
            SymbolInfo *currentSymbolInfo = hashTable[bucketNumber];
            SymbolInfo *previousSymbolInfo = nullptr;
            while (currentSymbolInfo != nullptr)
            {
                if (currentSymbolInfo->getName() == name)
                {
                    return nullptr; // Duplicate entry found
                }
                previousSymbolInfo = currentSymbolInfo;
                currentSymbolInfo = currentSymbolInfo->getNext();
            }
            previousSymbolInfo->setNext(newInfo);
            return newInfo;
        }
    }

    SymbolInfo *insertSymbolInfo(string name, string type)
    {
        SymbolInfo *info = new SymbolInfo(name, type);
        SymbolInfo *result = insertSymbolInfo(info);
        if (result == nullptr)
        {
            delete info; 
        }
        return result;
    }


    SymbolInfo *lookUpSymbolInfo(string name)
    {
        int bucketNumber = SDBMHash(name, bucketSize);
        SymbolInfo *currentSymbolInfo = hashTable[bucketNumber];
        while (currentSymbolInfo != nullptr)
        {
            if (currentSymbolInfo->getName() == name)
            {
                return currentSymbolInfo;
            }
            currentSymbolInfo = currentSymbolInfo->getNext();
        }
        return nullptr;
    }

    void printScopeTable()
    {
        cout << "ScopeTable # " << getScopeLabel() << endl;
        for (int i = 0; i < bucketSize; i++)
        {
            SymbolInfo *currentSymbolInfo = hashTable[i];
            if (currentSymbolInfo == nullptr)
                continue;

            cout << " " << i << " --> ";
            while (currentSymbolInfo != nullptr)
            {
                cout << "< " << currentSymbolInfo->getName() << " , " << currentSymbolInfo->getType() << " > ";
                currentSymbolInfo = currentSymbolInfo->getNext();
            }
            cout << endl;
        }
    }
};

#endif // _2205091_SCOPE_TABLE_HPP