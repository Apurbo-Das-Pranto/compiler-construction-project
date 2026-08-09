#!/bin/bash

# Usage: ./2205091_bash.sh input1.txt
# Run the 2205091_optimisedLexer on the given input file.
 
if [ -z "$1" ]; then
    echo "Usage: ./run.sh <input_file>"
    exit 1
fi
 
INPUT="$1"
BASE=$(basename "$INPUT" .txt)

 
flex -o lex.yy.c 2205091_optimisedLexer.l
g++ -std=c++17 -Wall lex.yy.c -o lexer -lfl
 
./lexer "$INPUT" "${BASE}_mytoken.txt" "${BASE}_mylog.txt"
 
echo "Done. Output files:"
echo "  ${BASE}_mytoken.txt"
echo "  ${BASE}_mylog.txt"
 
