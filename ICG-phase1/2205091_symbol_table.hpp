#ifndef _2205091_SYMBOL_TABLE_HPP
#define _2205091_SYMBOL_TABLE_HPP

#include <iostream>
#include <string>
#include "2205091_scope_table.hpp"

using namespace std;

class SymbolTable
{
private:
    ScopeTable *currentScopeTable;
    int bucketSize;
    int uniqueNumber;

public:
    SymbolTable(int bucketSize)
    {
        this->bucketSize = bucketSize;
        this->uniqueNumber = 1;
        this->currentScopeTable = new ScopeTable(bucketSize, uniqueNumber, nullptr);
    }

    ~SymbolTable()
    {
        while (currentScopeTable != nullptr)
        {
            ScopeTable *temp = currentScopeTable;
            currentScopeTable = currentScopeTable->getParentScopeTable();
            delete temp;
        }
    }

    ScopeTable *getCurrentScopeTable()
    {
        return currentScopeTable;
    }

    void enterScope()
    {
        uniqueNumber++;
        ScopeTable *newScopeTable = new ScopeTable(bucketSize, uniqueNumber, currentScopeTable);
        if (currentScopeTable != nullptr)
        {
            int childNum = currentScopeTable->getNextChildNumber();
            newScopeTable->setLabelNumber(childNum);
        }
        currentScopeTable = newScopeTable;
    }

    void exitScope(bool shouldPrint = true)
    {
        if (currentScopeTable->getParentScopeTable() != nullptr)
        {
            if (shouldPrint)
            {
                //printAllScopeTable();
            }
            ScopeTable *temp = currentScopeTable;
            currentScopeTable = currentScopeTable->getParentScopeTable();
            delete temp;
        }
    }

    SymbolInfo *insertSymbolInfo(SymbolInfo *info)
    {
        return currentScopeTable->insertSymbolInfo(info);
    }

    SymbolInfo *insertSymbolInfo(string name, string type)
    {
        return currentScopeTable->insertSymbolInfo(name, type);
    }

    SymbolInfo *lookUpCurrentScope(string name)
    {
        return currentScopeTable->lookUpSymbolInfo(name);
    }

    SymbolInfo *lookUpSymbolInfo(string name)
    {
        ScopeTable *temp = currentScopeTable;
        while (temp != nullptr)
        {
            SymbolInfo *symbolInfo = temp->lookUpSymbolInfo(name);
            if (symbolInfo != nullptr)
            {
                return symbolInfo;
            }
            temp = temp->getParentScopeTable();
        }
        return nullptr;
    }

    void printAllScopeTable()
    {
        cout << "\n\n\n";
        ScopeTable *temp = currentScopeTable;
        bool first = true;
        while (temp != nullptr)
        {
            if (!first)
                cout << "\n\n\n";
            temp->printScopeTable();
            temp = temp->getParentScopeTable();
            first = false;
        }
        cout << "\n\n";
    }
};

#endif // _2205091_SYMBOL_TABLE_HPP