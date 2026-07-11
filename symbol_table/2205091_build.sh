#!/bin/bash
# Build script for CSE310 Symbol Table assignment
# Student ID: 2205091
#
# Usage:
#   ./2205091_build.sh              -> compiles the project
#   ./2205091_symbol_table 2205091_sample_input.txt 2205091_output.txt   -> runs it after building
 
set -e
 
SRC="2205091_main.cpp"
OUT="2205091_symbol_table"
 
echo "Compiling Symbol Table implementation..."
g++ -std=c++17 -Wall -o "$OUT" "$SRC"
# for checking memory leaks,you may use the following cmd
# g++ -std=c++17 -Wall -g -fsanitize=address -o 2205091_symbol_table 2205091_main.cpp
 
echo "Build successful. Executable: ./$OUT"
echo "Usage: ./$OUT <input_file> <output_file>"
echo "executing the program with sample input and output files..."
./$OUT 2205091_sample_input.txt 2205091_output.txt
echo "Execution completed. Output written to 2205091_output.txt"
 
