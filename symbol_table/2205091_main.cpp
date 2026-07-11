#include<fstream>
#include<string>
#include<sstream>
#include"2205091_symbol_table.hpp"
#include<iostream>
using namespace std;

// Function to trim leading and trailing whitespace from a string
string trim(const string &str){
    size_t firstCharIndex=str.find_first_not_of(" \t\n\r");
    if(firstCharIndex==string::npos){
        return "";  
    }
    size_t lastCharIndex=str.find_last_not_of(" \t\n\r");
    return str.substr(firstCharIndex,lastCharIndex-firstCharIndex+1);
}
// Function to make even spacing between words in a string
string makeEvenSpacing(const string &str) {
    istringstream iss(str);
    string token, result;
    bool first = true;
    
    while (iss >> token) {  
        if (!first) {
            result += " ";  
        }
        result += token;
        first = false;
    }
    return result;
}


bool processCommand(string line,int cmd,SymbolTable &symbolTable,bool &shouldQuit){
    string trimmedLine=trim(line);
    if(trimmedLine.empty()){
        return false;
    }
    string evenSpacedLine=makeEvenSpacing(trimmedLine);

    istringstream iss(evenSpacedLine);
    string command;
    if(!(iss>>command)){
        return false;
    }
    if(command=="I"){
        string name,type;
        iss>>name>>type;
        if(type.empty()){
            cout<<"Error: Missing type for symbol '"<<name<<"' in command "<<cmd<<endl;
            return true;
        }else if(type=="FUNCTION"){
            string returnType;
            iss>>returnType;
            if(returnType.empty()){
                cout<<"Error: Missing return type for function '"<<name<<"' in command "<<cmd<<endl;
                return true;
            }
            string tempType=type+","+returnType+"<==(";
            string paramType;
            bool first=true;
            while(iss>>paramType){
                if(!first){
                    tempType+=",";
                }
                tempType+=paramType;
                first=false;
            }
            tempType+=")";
            type=tempType;

        }else if(type=="STRUCT" || type=="UNION"){
            string tempType;
            tempType=type+",{";
            string memberType;
            string memberName;
            bool first=true;
            while(iss>>memberType>>memberName){
                if(!first){
                    tempType+=",";
                }
                tempType+="("+memberType+","+memberName+")";
                first=false;
            }
            tempType+="}";
            type=tempType;
        }
        cout<<"Cmd "<<cmd<<": "<<evenSpacedLine<<endl;
        symbolTable.insertSymbolInfo(name,type);
        return true;
    }

    else if(command=="L"){
        cout<<"Cmd "<<cmd<<": "<<evenSpacedLine<<endl;
        string name;
        if(iss>>name){
            string extra;
            if(iss>>extra){
                cout<<"\tNumber of parameters mismatch for the command L"<<endl;
                return true;
            }
            else{
                SymbolInfo *symbolInfo=symbolTable.lookUpSymbolInfo(name);
                if(symbolInfo==nullptr){
                    cout<<"\t'"<<name<<"' not found in any of the ScopeTables"<<endl;
                }
                return true;
            }
        }
        else{
            cout<<"\tNumber of parameters mismatch for the command L"<<endl;
            return true;
        }
    }

    else if(command=="D"){
        cout<<"Cmd "<<cmd<<": "<<evenSpacedLine<<endl;
        string name;
        if(iss>>name){
            string extra;
            if(iss>>extra){
                cout<<"\tNumber of parameters mismatch for the command D"<<endl;
                return true;
            }else{
                symbolTable.removeSymbolInfo(name);
                return true;
            }
        }
        else{
            cout<<"\tNumber of parameters mismatch for the command D"<<endl;
            return true;
        }
    }

    else if(command=="P"){
        //2) Skip any invalid operand to the opcode "P" (for example P X)
         string option;
        if(iss>>option){
            string extra;
            if(iss>>extra){
                return false;
            }else{
                if(option=="A"){
                    cout<<"Cmd "<<cmd<<": "<<evenSpacedLine<<endl;
                    symbolTable.printAllScopeTable();
                    return true;
                }
                else if(option=="C"){
                    cout<<"Cmd "<<cmd<<": "<<evenSpacedLine<<endl;
                    symbolTable.printCurrentScopeTable();
                    return true;
                }
                else{
                    return false;
                }
            }
        }
        else{
            return false;
        }
    }

    else if(command=="S"){
        cout<<"Cmd "<<cmd<<": "<<evenSpacedLine<<endl;
        symbolTable.enterScope();
        return true;
    }

    else if(command=="E"){
        //3) Skip when trying to exit the root scope
        ScopeTable *temp=symbolTable.getCurrentScopeTable();
        if(temp->getParentScopeTable()==nullptr){
            return false;
        }
        else{
             cout<<"Cmd "<<cmd<<": "<<evenSpacedLine<<endl;
             symbolTable.exitScope();
             return true;
        }
    }

    else if(command=="Q"){
        cout<<"Cmd "<<cmd<<": "<<evenSpacedLine<<endl;
        symbolTable.deleteAllScopeTable();
        shouldQuit=true;
        return true;
    }

    else {
        return false;
    } 
}

int main(int arc,char *argv[]){
    if(arc!=3){
        cout<<"please provide input and output file name as command line argument"<<endl;
        return 0;
    }
    ifstream inputFile(argv[1]);
    ofstream outputFile(argv[2]);

    if(!inputFile.is_open()){
        cerr<<"Error: Could not open input file."<<endl;
        return 1;
    }
    if(!outputFile.is_open()){
        cerr<<"Error: Could not open output file."<<endl;
        return 1;
    }

    streambuf *originalCoutBuffer = cout.rdbuf();
    cout.rdbuf(outputFile.rdbuf());

    int bucketSize;
    inputFile>>bucketSize;
    SymbolTable symbolTable(bucketSize);
    string dummy;
    getline(inputFile,dummy);

    string line;
    int cmd=0;
    while(getline(inputFile,line)){
        if(line.empty()|| line.find_first_not_of(" \t\n\r")==string::npos){
            continue;
        }
        bool shouldQuit=false;
        if(processCommand(line, cmd+1, symbolTable, shouldQuit)){
            cmd++;
        }
        if(shouldQuit){
            break;
        }
    }
    cout.rdbuf(originalCoutBuffer);
    inputFile.close();
    outputFile.close();
}

